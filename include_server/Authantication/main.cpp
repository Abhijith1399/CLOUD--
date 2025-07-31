#include "../Authantication/Authantication.h"
#include <iostream>

int main() {
    Authentication auth("data/users.txt", "data/quota.txt", "data/logs/report.txt");

    std::string username, password;
    std::cout << "Enter username: "; std::cin >> username;
    std::cout << "Enter password: "; std::cin >> password;

    if (!auth.login(username, password)) {
        std::cout << "Login failed. Exiting...\n";
        return 1;
    }

    auth.createUserDirectory(username);

    size_t fileSizeMB = 2; // example file upload size

    if (auth.checkQuota(username, fileSizeMB)) {
        std::cout << "Upload allowed.\n";
        // Proceed upload...
        auth.logAction(username, "UPLOAD", "SUCCESS");
    }
    else {
        std::cout << "Upload denied. Quota exceeded or no quota.\n";
        // Deny upload...
    }

    // Example of other actions logging
    auth.logAction(username, "DOWNLOAD", "SUCCESS");
    auth.logAction(username, "DELETE", "SUCCESS");

    return 0;
}
