
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include"MemberB/ClientDownload.h"

using namespace std;
namespace fs = std::filesystem;

void downloadFileFromServer(SOCKET sock, const string& username, const string& filename) {
    // Step 1: Send download request
    string request = "DOWNLOAD|" + username + "|" + filename + "|";  
    if (send(sock, request.c_str(), (int)request.size(), 0) == SOCKET_ERROR) {
        cout << "Failed to send download request. Error: " << WSAGetLastError() << "\n";
        return;
    }

    // Step 2: Receive header response
    char header[512] = {};
    int headerLen = recv(sock, header, sizeof(header) - 1, 0);
    if (headerLen <= 0) {
        cout << "No response from server. Error: " << WSAGetLastError() << "\n";
        return;
    }
    header[headerLen] = '\0';
    string headerStr(header);

    // Step 3: Verify header format, expected: "FILE|filename|size|"
    if (headerStr.rfind("FILE|", 0) == 0) {  // starts with FILE|
        stringstream ss(headerStr);
        string tag, fname, sizeStr;
        getline(ss, tag, '|');
        getline(ss, fname, '|');
        getline(ss, sizeStr, '|');

        // Parse file size as size_t (supports very large files)
        size_t fileSize = 0;
        try {
            fileSize = stoull(sizeStr);
        }
        catch (...) {
            cout << "Invalid file size received from server.\n";
            return;
        }

        cout << "Downloading: " << fname << " (" << fileSize << " bytes)\n";

        // Step 4: Prepare local destination path: downloads/username/filename
        fs::path destDir = fs::path("downloads") / username;
        fs::create_directories(destDir);
        fs::path destFile = destDir / fname;

        ofstream fout(destFile, ios::binary);
        if (!fout) {
            cout << "Failed to open file for writing: " << destFile.string() << "\n";
            return;
        }

        // Step 5: Receive the file content from the server
        const size_t bufferSize = 4096;
        char buffer[bufferSize];
        size_t totalReceived = 0;

        while (totalReceived < fileSize) {
            // Calculate bytes to read, capped by buffer size
            int bytesToRead = (int)min(bufferSize, fileSize - totalReceived);
            int bytesReceived = recv(sock, buffer, bytesToRead, 0);

            if (bytesReceived <= 0) {
                cout << "Connection lost during file download.\n";
                break;
            }

            fout.write(buffer, bytesReceived);
            totalReceived += bytesReceived;
        }

        fout.close();

        // Step 6: Check completeness and report back
        if (totalReceived == fileSize) {
            cout << "Download completed: " << destFile.string() << "\n";
        }
        else {
            // Delete incomplete file
            fs::remove(destFile);
            cout << "Incomplete download. Expected " << fileSize << " bytes, got " << totalReceived << "\n";
        }
    }
    else {
        // If server replied with an error message or unexpected header
        cout << "Server error or unexpected response: " << headerStr << "\n";
    }
}
