#include "LogReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <iomanip>

using namespace std;
namespace fs = filesystem;
//store log reading
LogReader::LogReader(const string& basePath) : basePath(basePath) {}
//show user activity
void LogReader::showUserActivity(const string& username) {
    cout << "\nUser Activity Log for: " << username << "\n";

    // Helper function to display file access errors
    auto showFileError = [](const string& path) {
        cout << " - Error: Could not open file at " << path << "\n";
        cout << " - Current working directory: " << fs::current_path().string() << "\n";
        if (!fs::exists(path)) {
            cout << " - File does not exist\n";
        }
        else {
            cout << " - File exists but cannot be opened (permission issue?)\n";
        }
        };

    // --------- Login Log  data -----------------------------
    string loginPath = basePath + "loginlog.txt";
    ifstream loginFile(loginPath);
    cout << "\nLogin History:\n";
    if (loginFile.is_open()) {
        string line;
        while (getline(loginFile, line)) {
            if (line.find(username) != string::npos)
                cout << " - " << line << "\n";
        }
    }
    else {
        showFileError(loginPath);
    }

    // --------- reading Upload Log data --------------------------
    string uploadPath = basePath + "upload.log";
    ifstream uploadFile(uploadPath);
    cout << "\nUpload History:\n";
    if (uploadFile.is_open()) {
        string line;
        while (getline(uploadFile, line)) {
            if (line.find(username) != string::npos)
                cout << " - " << line << "\n";
        }
    }
    else {
        showFileError(uploadPath);
    }

    // --------- Quota File ------------
    string quotaPath = "data/quotas.txt";
    ifstream quotaFile(quotaPath);
    cout << "\nQuota Usage:\n";
    if (quotaFile.is_open()) {
        string line;
        bool found = false;
        while (getline(quotaFile, line)) {
            istringstream iss(line);
            string user;
            double usedMB;

            if (iss >> user >> usedMB) {
                if (user == username) {
                    cout << " - Used: " << fixed << setprecision(2) << usedMB << " MB\n";
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            cout << " - No quota data found for user '" << username << "'\n";
        }
    }
    else {
        showFileError(quotaPath);
    }

    // --------- Trash Folder Deletions --------
    string trashPath = "storage/" + username + "/trash/";
    cout << "\nTrash Deletion Logs:\n";
    if (fs::exists(trashPath)) {
        for (const auto& file : fs::directory_iterator(trashPath)) {
            if (!file.is_regular_file()) continue;

            auto ftime = fs::last_write_time(file);
            auto sys_time = chrono::system_clock::now() + (ftime - fs::file_time_type::clock::now());
            time_t cftime = chrono::system_clock::to_time_t(sys_time);

            tm tm_buf{};
            localtime_s(&tm_buf, &cftime);

            cout << " - " << file.path().filename().string()
                << " (Deleted on: " << put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << ")\n";
        }
    }
    else {
        cout << " - No trash folder found at " << trashPath << "\n";
    }
}