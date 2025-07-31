
#pragma once
#include <string>

class QuotaManager
{
private:
    size_t quotaLimit;
public:
    // Modified constructor to accept optional quota limit
    explicit QuotaManager(size_t limit = 100 * 1024 * 1024);  // Default 100MB

    size_t getUsedQuota(const std::string& username) const;
    bool canUpload(const std::string& username, size_t fileSize) const;
};