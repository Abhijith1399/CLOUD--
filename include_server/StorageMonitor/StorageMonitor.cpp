#include "../include/StorageMonitor/StorageMonitor.h"
#include <unordered_map>
#include <fstream>
#include <mutex>
#include <filesystem>

namespace fs = std::filesystem;

// Global in-memory quota map and mutex
static std::unordered_map<std::string, size_t> userQuota;
static std::mutex quotaMutex;

StorageMonitor::StorageMonitor(const std::string& quotaFile, size_t totalQuotaMB)
    : m_QuotaFile(quotaFile), m_TotalQuotaMB(totalQuotaMB)
{
    // Ensure directory exists
    fs::create_directories(fs::path(m_QuotaFile).parent_path());

    // Load quotas from file
    std::ifstream fin(m_QuotaFile);
    std::string user;
    size_t usedMB;
    while (fin >> user >> usedMB) {
        userQuota[user] = usedMB;
    }
}

// Add new user to quota list
void StorageMonitor::addUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(quotaMutex);
    if (userQuota.find(username) == userQuota.end()) {
        userQuota[username] = 0;
        std::ofstream fout(m_QuotaFile, std::ios::app);
        fout << username << " 0\n";
    }
}

bool StorageMonitor::canUpload(const std::string& username, size_t fileSize) {
    std::lock_guard<std::mutex> lock(quotaMutex);
    size_t usedMB = userQuota[username];
    size_t fileMB = (fileSize + (1 << 20) - 1) >> 20; // Round up to MB
    return (usedMB + fileMB) <= 100; // Fixed quota size per user
}

bool StorageMonitor::updateQuotaOnUpload(const std::string& username, size_t fileSize) {
    std::lock_guard<std::mutex> lock(quotaMutex);
    size_t fileMB = (fileSize + (1 << 20) - 1) >> 20; // Round up

    userQuota[username] += fileMB;
    // Save to quota file
    std::ofstream fout(m_QuotaFile);
    for (const auto& [user, used] : userQuota) {
        fout << user << " " << used << "\n";
    }
    return true;
}

bool StorageMonitor::updateQuotaOnDelete(const std::string& username, size_t fileSize) {
    std::lock_guard<std::mutex> lock(quotaMutex);
    size_t fileMB = (fileSize + (1 << 20) - 1) >> 20;

    if (userQuota.find(username) != userQuota.end()) {
        if (userQuota[username] >= fileMB)
            userQuota[username] -= fileMB;
        else
            userQuota[username] = 0;  // Prevent negative quota
    }

    // Save to quota file
    std::ofstream fout(m_QuotaFile);
    for (const auto& [user, used] : userQuota) {
        fout << user << " " << used << "\n";
    }
    return true;
}


bool StorageMonitor::updateQuotaOnRestore(const std::string& username, size_t fileSizeBytes) {
    std::lock_guard<std::mutex> lock(quotaMutex);
    size_t addMB = (fileSizeBytes + (1 << 20) - 1) >> 20; // round up to MB

    userQuota[username] += addMB;

    // Save updated quotas to file
    std::ofstream fout(m_QuotaFile);
    for (auto& p : userQuota) {
        fout << p.first << " " << p.second << "\n";
    }
    return true;
}
