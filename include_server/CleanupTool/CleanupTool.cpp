#include "CleanupTool.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstdlib>

using namespace std;
namespace fs = std::filesystem;

// Ensure the file is exists
void CleanupTool::ensureDirectoryExists(const std::string& path) {
    if (!fs::exists(path)) {
        fs::create_directories(path);
    }
}

// Get current the current time 
string CleanupTool::getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    tm tm_buf;
    localtime_s(&tm_buf, &now_time);
    ostringstream oss;
    oss << put_time(&tm_buf, "%Y%m%d_%H%M%S");
    return oss.str();
}

// Format file  size human-readable
string CleanupTool::formatFileSize(size_t bytes) {
    constexpr size_t KB = 1024;
    constexpr size_t MB = KB * 1024;

    if (bytes >= MB) {
        double sizeMB = static_cast<double>(bytes) / MB;
        stringstream ss;
        ss << fixed << setprecision(1) << sizeMB << "MB";
        return ss.str();
    }
    else if (bytes >= KB) {
        double sizeKB = static_cast<double>(bytes) / KB;
        stringstream ss;
        ss << fixed << setprecision(1) << sizeKB << "KB";
        return ss.str();
    }
    return to_string(bytes) + "B";
}

// Calculate file size
size_t CleanupTool::calculateDirectorySize(const std::string& path) {
    size_t totalSize = 0;
    try {
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                totalSize += entry.file_size();
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        cerr << "Error calculating size: " << e.what() << endl;
    }
    return totalSize;
}

// Automatic storage cleaning
void CleanupTool::autoCleanStorage() {
    ensureDirectoryExists(permanentPath);

    size_t currentSize = calculateDirectorySize(permanentPath);
    cout << "Storage status: " << formatFileSize(currentSize)
        << " / " << formatFileSize(storageLimit) << " ("
        << fixed << setprecision(1) << (static_cast<double>(currentSize) / storageLimit * 100) << "%)\n";

    if (currentSize >= storageLimit) {
        cout << "automatic cleanup...\n";
        deleteOldestFilesUntilUnderLimit();
    }
}

// Delete oldest files until under limit
void CleanupTool::deleteOldestFilesUntilUnderLimit() {
    vector<fs::directory_entry> allFiles;

    // Collect all files
    for (const auto& entry : fs::recursive_directory_iterator(permanentPath)) {
        if (entry.is_regular_file()) {
            allFiles.push_back(entry);
        }
    }

    // modification time (oldest first)
    sort(allFiles.begin(), allFiles.end(), [](const auto& a, const auto& b) {
        return fs::last_write_time(a) < fs::last_write_time(b);
        });

    size_t currentSize = calculateDirectorySize(permanentPath);
    int deletedCount = 0;

    for (const auto& file : allFiles) {
        if (currentSize < storageLimit) break;

        size_t fileSize = file.file_size();
        try {
            fs::remove(file.path());
            currentSize -= fileSize;
            deletedCount++;

            // Get file modification time
            auto ftime = fs::last_write_time(file);
            auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(
                ftime - fs::file_time_type::clock::now() + chrono::system_clock::now());
            time_t cftime = chrono::system_clock::to_time_t(sctp);
            tm tm_buf;
            localtime_s(&tm_buf, &cftime);

            cout << "Deleted: " << file.path().filename().string()
                << " (" << formatFileSize(fileSize)
                << ", modified " << put_time(&tm_buf, "%Y-%m-%d") << ")\n";
        }
        catch (const fs::filesystem_error& e) {
            cerr << "Error deleting file: " << e.what() << endl;
        }
    }

    cout << "Deleted " << deletedCount << " files. New size: "
        << formatFileSize(currentSize) << "\n";
}

// Backup functions
void CleanupTool::createUserBackup(const string& username) {
    string userDir = backupRoot + username + "/";
    string backupDir = backupRoot + username + "/backups/";
    string backupFile = backupDir + "backup_" + getCurrentTimestamp() + ".zip";

    ensureDirectoryExists(backupDir);

    // Create zip backup (requires zip utility)
    string command = "zip -r \"" + backupFile + "\" \"" + userDir + "\"";
    int result = system(command.c_str());

    if (result == 0) {
        cout << "Backup created: " << backupFile << endl;
    }
    else {
        cerr << "Backup failed for user: " << username << endl;
    }
}

void CleanupTool::restoreFromBackup(const string& username, const string& backupName) {
    string backupFile = backupRoot + username + "/backups/" + backupName;
    string userDir = backupRoot + username + "/";

    if (!fs::exists(backupFile)) {
        cerr << "Backup not found: " << backupFile << endl;
        return;
    }

    // Restore from zip 
    string command = "unzip -o \"" + backupFile + "\" -d \"" + userDir + "\"";
    int result = system(command.c_str());

    if (result == 0) {
        cout << "Restored backup: " << backupName << " to " << userDir << endl;
    }
    else {
        cerr << "Restore failed for user: " << username << endl;
    }
}

void CleanupTool::listUserBackups(const string& username) {
    string backupDir = backupRoot + username + "/backups/";

    if (!fs::exists(backupDir)) {
        cout << "No backups found for " << username << endl;
        return;
    }

    cout << "\nBackups for " << username << ":\n";
    cout << "--------------------------------\n";

    for (const auto& entry : fs::directory_iterator(backupDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".zip") {
            auto ftime = fs::last_write_time(entry);
            auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(
                ftime - fs::file_time_type::clock::now() + chrono::system_clock::now());
            time_t cftime = chrono::system_clock::to_time_t(sctp);
            tm tm_buf;
            localtime_s(&tm_buf, &cftime);

            cout << entry.path().filename().string()
                << " (" << formatFileSize(entry.file_size())
                << ", created " << put_time(&tm_buf, "%Y-%m-%d %H:%M") << ")\n";
        }
    }
    cout << "--------------------------------\n";
}

// Set storage limit
void CleanupTool::setStorageLimit(size_t mbLimit) {
    storageLimit = mbLimit * 1024 * 1024;
    cout << "Storage limit set to " << mbLimit << "MB\n";
}

// Get current storage usage
size_t CleanupTool::getCurrentStorageUsage() {
    return calculateDirectorySize(permanentPath);
}

// Get storage usage percentage
double CleanupTool::getStorageUsagePercentage() {
    size_t currentSize = calculateDirectorySize(permanentPath);
    return (static_cast<double>(currentSize) / storageLimit) * 100;
}