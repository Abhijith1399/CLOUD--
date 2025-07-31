#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// Member A
#include "MemberA/AuthClient.h"
#include "MemberA/SessionManager.h"

// Member C
#include "MemberC/DeleteHandler.h"
#include "MemberC/RestoreHandler.h"
#include "MemberC/PermanentDeleteHandler.h"
#include "MemberC/SecureSharing.h"
#include "MemberC/SecurityCheck.h"
#include "MemberC/Inbox.h"
#include "MemberC/TrashViewer.h"

using namespace std;

void showMemberCFeatures(const string& username) {
    DeleteHandler del;
    RestoreHandler res;
    PermanentDeleteHandler perDel;
    SecureSharing sec;
    SecurityCheck secheck;
    Inbox inb;
    TrashViewer tr;
    SessionManager session;

    int n;
    string file, tsendr;

    while (true) {
        cout << "\n--- CloudVault File Operations ---\n";
        cout << "1. Move file to Trash\n";
        cout << "2. Restore file\n";
        cout << "3. Permanent Delete\n";
        cout << "4. Share File Securely\n";
        cout << "5. View Inbox\n";
        cout << "6. View Trash\n";
        cout << "7. Logout\n";
        cout << "Enter choice: ";
        cin >> n;

        switch (n) {
        case 1:
            cout << "Enter filename to move to trash: ";
            cin >> file;
            del.deleteFile(username, file);
            break;

        case 2:
            cout << "Enter filename to restore: ";
            cin >> file;
            res.restoreFile(username, file);
            break;

        case 3:
            cout << "Enter filename to delete permanently: ";
            cin >> file;
            perDel.deletePermanently(username, file);
            break;

        case 4:
            if (!SecurityCheck::isLoggedIn(username)) {
                SecurityCheck::loginUser(username);
            }
            cout << "Enter receiver username: ";
            cin >> tsendr;
            cout << "Enter filename to share: ";
            cin >> file;
            sec.shareFile(username, tsendr, file);
            break;

        case 5:
            inb.listInbox(username);
            break;

        case 6:
            tr.listTrash(username);
            break;

        case 7:
            cout << "Logging out...\n";
        session.endSession(username);
            SecurityCheck::logoutUser(username);
            return;

        default:
            cout << "Invalid option.\n";
        }

        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

int main() {

    SessionManager session;

    AuthClient auth("", "");
    string username, password, email, phone, address, name;
    int choice;

    do {
        cout << "\n=== CloudVault Client Menu ===\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            cout << "Enter full name: ";
            getline(cin, name);
            cout << "Enter username: ";
            getline(cin, username);
            cout << "Enter password: ";
            getline(cin, password);
            cout << "Enter email: ";
            getline(cin, email);
            cout << "Enter phone: ";
            getline(cin, phone);
            cout << "Enter address: ";
            getline(cin, address);

            if (auth.registerUser(username, password, email, address, phone, name)) {
                cout << "✅ Registered successfully.\n";
            }
            else {
                cout << "❌ Registration failed.\n";
            }
            break;

        case 2:
            cout << "Enter username: ";
            getline(cin, username);
            cout << "Enter password: ";
            getline(cin, password);

            if (auth.login(username, password)) {
                cout << "✅ Login successful. Welcome, " << username << "!\n";
               session.createSession(username);
                SecurityCheck::loginUser(username);
                showMemberCFeatures(username);
            }
            else {
                cout << "❌ Login failed.\n";
            }
            break;

        case 3:
            cout << "👋 Exiting CloudVault...\n";
            break;

        default:
            cout << "⚠️ Invalid choice. Try again.\n";
        }

    } while (choice != 3);

    return 0;
}
