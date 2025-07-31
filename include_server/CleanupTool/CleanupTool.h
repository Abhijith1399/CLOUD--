#pragma once
#include <string>
#include <vector>
#include <chrono>

class CleanupTool {
public:
    // Automatic cleaning
    void autoCleanStorage();
    void setStorageLimit(size_t mbLimit);

    // Backup functionality
    void createUserBackup(const std::string& username);
    void restoreFromBackup(const std::string& username, const std::string& backupName);
    void listUserBackups(const std::string& username);

    // Monitoring
    size_t getCurrentStorageUsage();
    double getStorageUsagePercentage();

private:
    const size_t DEFAULT_STORAGE_LIMIT = 50 * 1024 * 1024; // 50MB
    size_t storageLimit = DEFAULT_STORAGE_LIMIT;
    std::string permanentPath = "Trash/permanentDelete/";
    std::string backupRoot = "storage/";

    // Helper functions
    size_t calculateDirectorySize(const std::string& path);
    void deleteOldestFilesUntilUnderLimit();
    std::string formatFileSize(size_t bytes);
    std::string getCurrentTimestamp();
    void ensureDirectoryExists(const std::string& path);
};