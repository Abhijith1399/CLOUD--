#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#undef byte

#include <iostream>
#include "MemberC/RestoreHandler.h"
#include "../features/NetworkUtils.h"
#include <filesystem>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;
namespace fs = std::filesystem;

void RestoreHandler::setUsername(string username) {
    this->username = username;
}

string RestoreHandler::getUsername() {
    return this->username;
}

void RestoreHandler::setFilename(string filename) {
    this->filename = filename;
}

string RestoreHandler::getFilename() {
    return this->filename;
}

void RestoreHandler::restoreFile() {
    // For multithreading support
    thread([=]() {
        lock_guard<mutex> lock(restoreMutex);

        string request = "RESTORE|" + username + "|" + filename;
        string response;

        cout << "Sending restore request to server...\n";

   if (sendToServer(request, response)) {
            cout << "Server Response: " << response << "\n";

            string userDir = "storage/" + username + "/";
            string trashPath = userDir + "trash/" + filename;
            string restorePath = userDir + filename;

            try {
                if (fs::exists(trashPath)) {
                    fs::rename(trashPath, restorePath);
                    cout << "File restored from client trash.\n";
                }
         else {
                    cout << "File not found in client trash: " << trashPath << "\n";
                }
            }
            catch (const exception& e) {
                cerr << " Error during local restore: " << e.what() << "\n";
            }

        }
        else {
            cout << "Failed to connect to server for restore.\n";
        }

        }).detach(); // Allows independent execution
}

void RestoreHandler::restoreFile(const string& username, const string& filename) {
    setUsername(username);
    setFilename(filename);
    restoreFile();  // Call class version
}

