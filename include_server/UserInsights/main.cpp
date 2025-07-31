#include "../include/UserInsights/UserInsights.h"
#include <iostream>

int main() {
    UserInsight insight(
        "data/users.txt",          // Pipe-separated, first field is username
        "data/logs/loginlog.txt",  // Format: [timestamp] Username: xxx - Login Status
        "data/logs/reports.txt",   // Format: [timestamp] REGISTER: User=xxx Result=STATUS
        "data/logs/upload.log",    // Pipe-separated, first field is username
        "data/quotas.txt"         // Space-separated: username used total
    );

    while (true) {
        std::string username;
        std::cout << "Enter username to get insights (or blank to exit): ";
        std::getline(std::cin, username);
        if (username.empty()) break;

        insight.showUserDetails(username);
    }

    std::cout << "Exiting.\n";
    return 0;
}