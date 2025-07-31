#include "UserInsights/UserInsights.h"
#include "../include/UserInsights/UserInsights.h"
#include "../include/CleanupTool/CleanerBase.h"
#include "../include/CleanupTool/CleanerBase.h"
#include "AdminPanel/AdminPanel.h"
#include"../include/CleanupTool/CleanupTool.h"
#include "../include/LogReader/LogReader.h"//
#include <iostream>
#include <string>
using namespace std;
int main() {
    UserInsight insight("data/users.txt", "data/logs/loginlog.txt",
        "data/logs/reports.txt", "data/logs/upload.log",
        "data/quotas.txt");
    LogReader logReader;
    CleanupTool cleaner;
    AdminPanel admin;

    while (true) {
        std::cout << "\n=== Main Menu ===\n";
        std::cout << "1. User Insights\n";
        std::cout << "2. User Activity Log\n";
        std::cout << "3. Cleanup Tool\n";
        std::cout << "4. Admin Panel\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter choice: ";

        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore();
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 5) break;

        std::string username;
        switch (choice) {
        case 1:
            std::cout << "Enter username for insights (blank = cancel): ";
            std::getline(std::cin, username);
            if (!username.empty())
                insight.showUserDetails(username);
            break;
        case 2:
            std::cout << "Enter username to view activity (blank = cancel): ";
            std::getline(std::cin, username);
            if (!username.empty())
                logReader.showUserActivity(username);
            break;
        case 3:
        {
            bool keep = true;
            while (keep) {
                std::cout << "\n-- Cleanup Menu --\n"
                    << "1. Automatic Cleanup\n"
                    << "2. Create User Backup\n"
                    << "3. List Backups\n"
                    << "4. Restore Backup\n"
                    << "5. Set Storage Limit\n"
                    << "6. Back to Main Menu\n"
                    << "Choice: ";
                int c2 = 0;
                if (!(std::cin >> c2)) { std::cin.clear(); std::cin.ignore(); continue; }
                std::cin.ignore();

                switch (c2) {
                case 1: cleaner.autoCleanStorage(); break;
                case 2:
                    std::cout << "Username: ";
                    std::getline(std::cin, username);
                    cleaner.createUserBackup(username);
                    break;
                case 3:
                    std::cout << "Username: ";
                    std::getline(std::cin, username);
                    cleaner.listUserBackups(username);
                    break;
                case 4:
                    std::cout << "Username: ";
                    std::getline(std::cin, username);
                    std::cout << "Backup file: ";
                    {
                        std::string bk;
                        std::getline(std::cin, bk);
                        cleaner.restoreFromBackup(username, bk);
                    }
                    break;
                case 5:
                    std::cout << "New limit (MB): ";
                    size_t mb;
                    if (std::cin >> mb) {
                        cleaner.setStorageLimit(mb);
                    }
                    std::cin.ignore();
                    break;
                case 6:
                    keep = false;
                    break;
                default:
                    std::cout << "Invalid option\n";
                }
            }
        }
        break;
        case 4:
        {
            std::cout << "Admin login\nUsername: ";
            std::string user, pass;
            std::cin >> user >> pass;
            if (admin.validate(user, pass)) {
                std::cout << "Login successful\n";
                admin.showDashboard();
            }
            else {
                std::cout << "Access denied\n";
            }
        }
        break;
        default:
            std::cout << "Invalid choice\n";
        }
        std::cout << "\n(Press Enter to continue...)";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << "Exiting.\n";
    return 0;
}
