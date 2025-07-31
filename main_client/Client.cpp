#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX  // Disables Windows min/max macros

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <string>
#include <limits>
#include <vector>
#include <mutex>
#include <unordered_set>

#include "NetworkUtils.h"

// Member A headers
#include "MemberA/AuthClient.h"
#include "MemberA/SessionManager.h"
#include "MemberA/ConnectionManager.h"

// Member B headers
#include"MemberB/ClientUpload.h"
#include "MemberB/UploadHandler.h"
#include "MemberB/DownloadHandler.h"
#include "MemberB/FileLister.h"
#include "MemberB/QuotaManager.h"

// Member C headers
#include "MemberC/DeleteHandler.h"
#include "MemberC/RestoreHandler.h"
#include "MemberC/PermanentDeleteHandler.h"
#include "MemberC/SecureSharing.h"
#include "MemberC/SecurityCheck.h"
#include "MemberC/Inbox.h"
#include "MemberC/TrashViewer.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;
namespace fs = filesystem;

// global configuration
const string SERVER_IP = "172.19.7.179";
const int SERVER_PORT = 8080;
const size_t MAX_QUOTA = 100 * 1024 * 1024; // for 100MB per user


bool uploadFileToServer(SOCKET sock, const string& username, const string& filepath);
void downloadFileFromServer(SOCKET sock, const string& username, const string& filename);

void showFeaturesMenu(const string& username) {


    UploadHandler uploader;
    DownloadHandler downloader;
    FileLister lister;
    QuotaManager quotaManager(MAX_QUOTA);

    SessionManager sessionMgr;

    DeleteHandler del;
    RestoreHandler res;
    PermanentDeleteHandler perDel;
    SecureSharing sec;
    Inbox inb;
    TrashViewer tr;

    int choice;
    string file, target;

    while (true) {
        cout << "\n=== Cloud ++ Features Menu ===\n";
        cout << "1. Upload File\n2. Download File\n3. List My Files\n";
        cout << "4. Move file to Trash\n5. Restore file\n6. Permanent Delete\n";
        cout << "7. Share File Securely\n8. View Inbox\n9. View Trash\n";
        cout << "10. Logout\nEnter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1: { // Upload
            cout << "Enter local file path to upload: ";
            getline(cin, file);

            if (!fs::exists(file) || !fs::is_regular_file(file)) {
                cout << "File does not exist or is not a regular file.\n";
                break;
            }

            size_t fileSize = fs::file_size(file);

            if (!quotaManager.canUpload(username, fileSize)) {
                cout << "Quota exceeded (max 100MB).\n";
                break;
            }

            ConnectionManager connMgr;
            int sock = connMgr.connectToServer(SERVER_IP, SERVER_PORT);
            if (sock < 0) {
                cout << "Failed to connect to server for upload.\n";
                break;
            }

            if (uploadFileToServer(sock, username, file)) {
                cout << "Upload successful!\n";
            }
            else {
                cout << "Upload failed.\n";
            }

            closesocket(sock);
            break;
        }

        case 2: { // Download
            cout << "Enter filename to download: ";
            getline(cin, file);

            if (file.empty()) {
                cout << "Filename cannot be empty.\n";
                break;
            }

            ConnectionManager connMgr;
            int sock = connMgr.connectToServer(SERVER_IP, SERVER_PORT);
            if (sock < 0) {
                cout << "Failed to connect to server for download.\n";
                break;
            }

            downloadFileFromServer(sock, username, file);

            closesocket(sock);
            break;
        }

        case 3: { // List files
            string request = "LIST|" + username;
            string response;
            if (sendToServer(request, response)) {
                cout << "Your files:\n" << response << "\n";
            }
            else {
                cout << "Failed to get file list from server.\n";
            }
            break;
        }

        case 4: { // Move file to trash
            cout << "Enter filename to move to Trash: ";
            getline(cin, file);
            del.deleteFile(username, file);
            break;
        }

        case 5: { // Restore file
            cout << "Enter filename to restore: ";
            getline(cin, file);
            res.restoreFile(username, file);
            break;
        }

        case 6: { // Permanent delete
            cout << "Enter filename to permanently delete: ";
            getline(cin, file);
            perDel.deletePermanently(username, file);
            break;
        }


        case 7: { // Share file securely
            cout << "Enter receiver username: ";
            getline(cin, target);
            cout << "Enter filename to share: ";
            getline(cin, file);

            if (sec.shareFile(username, target, file)) {
                cout << "File shared successfully.\n";
            }
            else {
                cout << "Failed to share file.\n";
            }
            break;
        }


        case 8: { // View inbox
            inb.listInbox(username);
            break;
        }

        case 9: { // View trash
            tr.listTrash(username);
            break;
        }

        case 10: { // Logout
            cout << "Logging out...\n";
            string request = "LOGOUT|" + username;
            string response;
            sendToServer(request, response);
            SecurityCheck::logoutUser(username);
            sessionMgr.endSession(username);
            return;
        }

        default:
            cout << "Invalid option.\n";
        }
    }
}

int main() {
    AuthClient auth;

    SessionManager sessionMgr;
    
    string username, password;
    int choice;

    do {
        cout << "\n=== Cloud++ ===\n1. Register\n2. Login\n3. Exit\nEnter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string request, response;

        switch (choice) {
        case 1: { // Register
            string name, email, phone, address;
            cout << "Full name: "; getline(cin, name);

            cout << "Username: "; getline(cin, username);
            cout << "Password: "; getline(cin, password);
            cout << "Email: "; getline(cin, email);
            cout << "Phone: "; getline(cin, phone);
            cout << "Address: "; getline(cin, address);

            // Password hashing can be done here or server side, depending on your design
            request = "REGISTER|" + name + "|" + username + "|" + password + "|" +
                email + "|" + phone + "|" + address;

            if (sendToServer(request, response)) {
                cout << "Server: " << response << "\n";
            }
            else {
                cout << "Error: Unable to connect to server at " << SERVER_IP << ":" << SERVER_PORT << ". Please check your network.\n";
            }

            if (response == "REGISTER_SUCCESS") {
                // Create client storage folders storage/<username>/inbox and trash
                fs::path clientDataDir = fs::path("storage") / username;
                fs::create_directories(clientDataDir / "trash");
                fs::create_directories(clientDataDir / "inbox");

                cout << "Registration successful. Local storage folders created for user.\n";
            }

            break;
        }

        case 2: { // Login
            cout << "Username: "; getline(cin, username);
            cout << "Password: "; getline(cin, password);

            
            request = "LOGIN|" + username + "|" + password;

            if (sendToServer(request, response)) {
                if (response.find("SUCCESS") != string::npos) {
                    cout << "Login successful. Welcome " << username << "!\n";

                    SecurityCheck::loginUser(username);
                    sessionMgr.createSession(username);

                    showFeaturesMenu(username);
                }
                else {
                    cout << "Login failed: " << response << "\n";
                }
            }
            else {
                cout << "Error: Unable to connect to server." << endl;
            }
            break;
        }

        case 3:
            cout << "Exiting..." << endl;
            break;

        default:
            cout << "Invalid choice" << endl;
        }
    } while (choice != 3);

    return 0;
}
