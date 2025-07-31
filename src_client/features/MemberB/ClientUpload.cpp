
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

using namespace std;
namespace fs = std::filesystem;

const size_t BUFFER_SIZE = 8192;

// Upload File to Server
bool uploadFileToServer(SOCKET sock, const string& username, const string& filepath) {
    if (!fs::exists(filepath) || !fs::is_regular_file(filepath)) {
        cout << "File does not exist or is not a regular file.\n";
        return false;
    }

    ifstream file(filepath, ios::binary | ios::ate);
    if (!file) {
        cout << "Failed to open file.\n";
        return false;
    }

    size_t fileSize = file.tellg();
    file.seekg(0, ios::beg);
    string filename = fs::path(filepath).filename().string();

    // Step 1: Send header
    string header = "UPLOAD|" + username + "|" + filename + "|" + to_string(fileSize) + "|";
    cout << "[DEBUG] Sending header: '" << header << "'" << endl;
    if (send(sock, header.c_str(), static_cast<int>(header.size()), 0) == SOCKET_ERROR) {
        cout << "Failed to send upload header. Error: " << WSAGetLastError() << "\n";
        return false;
    }

    // Step 2: Wait for server READY
    char respBuf[1024] = { 0 };
    int r = recv(sock, respBuf, sizeof(respBuf) - 1, 0);
    if (r <= 0) {
        cout << "No response from server. Error: " << WSAGetLastError() << "\n";
        return false;
    }
    respBuf[r] = '\0';
    string serverReply = respBuf;
    cout << "[DEBUG] Server reply: '" << serverReply << "'" << endl;

    if (serverReply != "READY") {
        cout << "Server rejected upload: " << serverReply << "\n";
        return false;
    }

    // Step 3: Send file data
    vector<char> buffer(BUFFER_SIZE);
    while (!file.eof()) {
        file.read(buffer.data(), BUFFER_SIZE);
        streamsize bytesRead = file.gcount();
        if (bytesRead <= 0) break;

        int sent = send(sock, buffer.data(), static_cast<int>(bytesRead), 0);
        if (sent == SOCKET_ERROR) {
            cout << "Upload interrupted. Error: " << WSAGetLastError() << "\n";
            return false;
        }
    }

    // Step 4: Final confirmation
    r = recv(sock, respBuf, sizeof(respBuf) - 1, 0);
    if (r > 0) {
        respBuf[r] = '\0';
        string finalReply = respBuf;
        cout << "[DEBUG] Final server response: '" << finalReply << "'" << endl;

        if (finalReply == "UPLOAD_SUCCESS") {
            // Save local copy
            try {
                fs::path localDir = fs::path("storage") / username;
                fs::create_directories(localDir);
                fs::copy_file(filepath, localDir / filename,
                    fs::copy_options::overwrite_existing);
                cout << "Local copy saved to: " << (localDir / filename).string() << "\n";
            }
            catch (const fs::filesystem_error& e) {
                cout << "Warning: Failed to save local copy: " << e.what() << "\n";
            }
            return true;
        }
        else {
            cout << "Upload failed: " << finalReply << "\n";
            return false;
        }
    }

    cout << "No confirmation from server. Upload failed.\n";
    return false;
}






