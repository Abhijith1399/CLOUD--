#include "../include/UserInsights/UserInsights.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>
#include <cctype>
#include <algorithm>

UserInsight::UserInsight(
    const std::string& usersP,
    const std::string& loginP,
    const std::string& registerP,
    const std::string& uploadP,
    const std::string& quotaP)
    : usersPath(usersP), loginPath(loginP), registerPath(registerP),
    uploadPath(uploadP), quotaPath(quotaP)
{}

std::string UserInsight::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool UserInsight::caseInsensitiveCompare(const std::string& a, const std::string& b) {
    return std::equal(a.begin(), a.end(), b.begin(), b.end(),
        [](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

std::string UserInsight::extractUsernameFromLogin(const std::string& line) {
    size_t userPos = line.find("Username: ");
    if (userPos == std::string::npos) return "";
    size_t start = userPos + 10;
    size_t end = line.find(" - ", start);
    if (end == std::string::npos) return "";
    return trim(line.substr(start, end - start));
}

std::string UserInsight::extractUsernameFromRegister(const std::string& line) {
    size_t userPos = line.find("User=");
    if (userPos == std::string::npos) return "";
    size_t start = userPos + 5;
    size_t end = line.find(" Result=", start);
    if (end == std::string::npos) end = line.length();
    return trim(line.substr(start, end - start));
}

std::string UserInsight::extractFirstField(const std::string& line) {
    size_t pipePos = line.find('|');
    return trim(pipePos != std::string::npos ? line.substr(0, pipePos) : line);
}

void UserInsight::showUserDetails(const std::string& username) {
    std::set<std::string> validUsers;

    // Get valid users from users.txt (pipe-separated first field)
    {
        std::ifstream fin(usersPath);
        if (!fin) {
            std::cerr << "Failed to open users file: " << usersPath << std::endl;
            return;
        }
        std::string line;
        while (std::getline(fin, line)) {
            std::string user = extractFirstField(line);
            if (!user.empty()) validUsers.insert(user);
        }
    }

    // Also check registration file
    {
        std::ifstream fin(registerPath);
        if (fin) {
            std::string line;
            while (std::getline(fin, line)) {
                std::string user = extractUsernameFromRegister(line);
                if (!user.empty()) validUsers.insert(user);
            }
        }
    }

    // Find matching user (case insensitive)
    bool found = false;
    std::string matchedUsername;
    for (const auto& validUser : validUsers) {
        if (caseInsensitiveCompare(validUser, username)) {
            found = true;
            matchedUsername = validUser;
            break;
        }
    }

    if (!found) {
        std::cout << "User '" << username << "' not found." << std::endl;
        return;
    }

    // Initialize counters
    int totalLogins = 0;
    int successfulLogins = 0;
    int failedLogins = 0;
    int totalUploads = 0;
    long quotaUsed = 0;
    long quotaTotal = 0;
    bool lowQuotaWarning = false;

    // Parse login log
    {
        std::ifstream fin(loginPath);
        if (!fin) {
            std::cerr << "Failed to open login log: " << loginPath << std::endl;
        }
        else {
            std::string line;
            while (std::getline(fin, line)) {
                std::string user = extractUsernameFromLogin(line);
                if (caseInsensitiveCompare(user, matchedUsername)) {
                    totalLogins++;
                    if (line.find("Login Success") != std::string::npos) {
                        successfulLogins++;
                    }
                    else if (line.find("Login Failed") != std::string::npos) {
                        failedLogins++;
                    }
                }
            }
        }
    }

    // Parse upload log
    {
        std::ifstream fin(uploadPath);
        if (!fin) {
            std::cerr << "Failed to open upload log: " << uploadPath << std::endl;
        }
        else {
            std::string line;
            while (std::getline(fin, line)) {
                std::string user = extractFirstField(line);
                if (caseInsensitiveCompare(user, matchedUsername)) {
                    totalUploads++;
                }
            }
        }
    }

    // Parse quota file
    {
        std::ifstream fin(quotaPath);
        if (!fin) {
            std::cerr << "Failed to open quota file: " << quotaPath << std::endl;
        }
        else {
            std::string line;
            while (std::getline(fin, line)) {
                std::istringstream iss(line);
                std::string user;
                long used, total;
                if (iss >> user >> used >> total) {
                    if (caseInsensitiveCompare(user, matchedUsername)) {
                        quotaUsed = used;
                        quotaTotal = total;
                        double percUsed = total > 0 ? (used * 100.0 / total) : 0.0;
                        lowQuotaWarning = (percUsed >= 75.0);
                        break;
                    }
                }
            }
        }
    }

    // Display results
    std::cout << "\nUser Insights for '" << matchedUsername << "':\n";
    std::cout << "----------------------------\n";
    std::cout << "Total logins: " << totalLogins << "\n";
    std::cout << "  Successful: " << successfulLogins << "\n";
    std::cout << "  Failed: " << failedLogins << "\n";
    std::cout << "Total uploads: " << totalUploads << "\n";
    std::cout << "Storage usage: " << formatBytes(quotaUsed) << " / " << formatBytes(quotaTotal) << " ("
        << std::fixed << std::setprecision(1) << (quotaTotal > 0 ? quotaUsed * 100.0 / quotaTotal : 0) << "%)\n";

    if (lowQuotaWarning) {
        std::cout << "\033[31mWARNING: Storage usage exceeds 75% of quota!\033[0m\n";
    }
}

std::string UserInsight::formatBytes(long bytes) {
    constexpr long KB = 1024;
    constexpr long MB = KB * 1024;
    constexpr long GB = MB * 1024;

    std::ostringstream oss;

    if (bytes >= GB)
        oss << std::fixed << std::setprecision(2) << (bytes / (double)GB) << " GB";
    else if (bytes >= MB)
        oss << std::fixed << std::setprecision(2) << (bytes / (double)MB) << " MB";
    else if (bytes >= KB)
        oss << std::fixed << std::setprecision(2) << (bytes / (double)KB) << " KB";
    else
        oss << bytes << " bytes";

    return oss.str();
}