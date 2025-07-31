
#include "MemberB/QuotaManager.h"
#include <filesystem>
#include <iostream>

using namespace std;

QuotaManager::QuotaManager(size_t limit) : quotaLimit(limit)
{}

size_t QuotaManager::getUsedQuota(const std::string& username) const
{
    filesystem::path userPath = "data/" + username;
    size_t totalSize = 0;

    try {
        if (filesystem::exists(userPath)) {
            for (const auto& file : filesystem::recursive_directory_iterator(userPath))
            {
                if (file.is_regular_file()) {
                    totalSize += file.file_size();
                }
            }
        }
    }
    catch (const filesystem::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
    }

    return totalSize;
}

bool QuotaManager::canUpload(const std::string& username, size_t fileSize) const
{
    return (getUsedQuota(username) + fileSize) <= quotaLimit;
}