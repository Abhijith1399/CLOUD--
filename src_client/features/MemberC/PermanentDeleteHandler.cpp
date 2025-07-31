#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#undef byte

#include <iostream>
#include "MemberC/PermanentDeleteHandler.h"
#include "../features/NetworkUtils.h"
#include <filesystem>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;
namespace fs = std::filesystem;

void PermanentDeleteHandler::setUsername(string username) {
    this->username = username;
}

string PermanentDeleteHandler::getUsername() {
    return this->username;
}

void PermanentDeleteHandler::setFilename(string filename) {
    this->filename = filename;
}

string PermanentDeleteHandler::getFilename() {
    return this->filename;
}

void PermanentDeleteHandler::deletePermanently() {
    thread([=]() {
        lock_guard<mutex> lock(perMutex);

        string request = "TRASH|" + username + "|" + filename;
        string response;

        cout << " Sending permanent delete request to server...\n";

        if (sendToServer(request, response)) {
            cout << " Server Response: " << response << "\n";

            string trashFile = "storage/" + username + "/trash/" + filename;

            try {
                if (fs::exists(trashFile)) {
                    fs::remove(trashFile);
                    cout << " File permanently deleted from client trash.\n";
                }
                else {
                    cout << " File not found in client trash: " << trashFile << "\n";
                }
            }
            catch (const exception& e) {
                cerr << " Client-side delete error: " << e.what() << "\n";
            }

        }
        else {
            cout << "Could not connect to server for permanent delete.\n";
        }

        }).detach();  // Runs in background
}

void PermanentDeleteHandler::deletePermanently(const string& username, const string& filename) {
    setUsername(username);
    setFilename(filename);
    deletePermanently();  // Calls internal threaded logic
}
