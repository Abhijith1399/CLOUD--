#ifndef USERINSIGHT_H
#define USERINSIGHT_H

#include <string>
#include <vector>

class UserInsight {
public:
    UserInsight(
        const std::string& usersPath = "data/users.txt",
        const std::string& loginPath = "data/logs/loginlog.txt",
        const std::string& registerPath = "data/logs/reports.txt",
        const std::string& uploadPath = "data/logs/upload.log",
        const std::string& quotaPath = "data/quotas.txt");

    void showUserDetails(const std::string& username);

private:
    std::string usersPath;
    std::string loginPath;
    std::string registerPath;
    std::string uploadPath;
    std::string quotaPath;

    std::string formatBytes(long bytes);
    std::string trim(const std::string& str);
    bool caseInsensitiveCompare(const std::string& a, const std::string& b);
    std::string extractUsernameFromLogin(const std::string& line);
    std::string extractUsernameFromRegister(const std::string& line);
    std::string extractFirstField(const std::string& line);
};

#endif // USERINSIGHT_H