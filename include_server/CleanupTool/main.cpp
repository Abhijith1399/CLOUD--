#include "CleanupTool.h"
#include <iostream>
#include <thread>

using namespace std;

void showMenu() {
    cout << "\n==== Cleanup Tool ====\n";
    cout << "1. Run Automatic Cleanup\n";
    cout << "2. Create User Backup\n";
    cout << "3. List Backups\n";
    cout << "4. Restore Backup\n";
    cout << "5. Set Storage Limit\n";
    cout << "6. Exit\n";
    cout << "Enter choice: ";
}

int main() {
    CleanupTool cleaner;
    string username;
    string backupName;
    int choice;
    size_t mbLimit;

    while (true) {
        showMenu();
        cin >> choice;
        cin.ignore(); // Clear newline

        switch (choice) {
        case 1:
            cleaner.autoCleanStorage();
            break;
        case 2:
            cout << "Enter username: ";
            getline(cin, username);
            cleaner.createUserBackup(username);
            break;
        case 3:
            cout << "Enter username: ";
            getline(cin, username);
            cleaner.listUserBackups(username);
            break;
        case 4:
            cout << "Enter username: ";
            getline(cin, username);
            cout << "Enter backup filename: ";
            getline(cin, backupName);
            cleaner.restoreFromBackup(username, backupName);
            break;
        case 5:
            cout << "Enter new limit in MB: ";
            cin >> mbLimit;
            cleaner.setStorageLimit(mbLimit);
            break;
        case 6:
            return 0;
        default:
            cout << "Invalid choice\n";
        }

        // Pause before continuing
        cout << "\nPress Enter to continue...";
        cin.ignore();
    }
}