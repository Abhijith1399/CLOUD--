#pragma once
#include <string>

class StorageMonitor {
public:
    StorageMonitor(const std::string& quotaFile = "data/quotas.txt",
        size_t totalQuotaMB = 500);

    void addUser(const std::string& username);
    bool canUpload(const std::string& username, size_t fileSize);     // Check if allowed to upload
    bool updateQuotaOnUpload(const std::string& username, size_t fileSize);
    bool updateQuotaOnDelete(const std::string& username, size_t fileSize);
    bool updateQuotaOnRestore(const std::string& username, size_t fileSizeBytes);

private:
    const std::string m_QuotaFile;
    size_t m_TotalQuotaMB;
};