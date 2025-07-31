#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "MemberB/UploadHandler.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include<sstream>
#include<string>

UploadHandler::UploadHandler() : interrupted(false) {}

bool UploadHandler::saveFile(int clientSocket, const std::string& path) {
    std::ofstream outFile(path, std::ios::binary);
    if (!outFile) return false;

    char buffer[4096];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        outFile.write(buffer, bytesReceived);
        if (bytesReceived == 0) {
            interrupted = true;
            break;
        }
    }

    return true;
}

bool UploadHandler::receiveFile(int clientSocket, const std::string& ) {
    char headerBuffer[512] = {};
    int bytes = recv(clientSocket, headerBuffer, sizeof(headerBuffer), 0);
    if (bytes <= 0) return false;

    std::string header(headerBuffer);
    std::istringstream ss(header);
    std::string command, username, filename, fileSizeStr;

    getline(ss, command, '|');
    getline(ss, username, '|');
    getline(ss, filename, '|');
    getline(ss, fileSizeStr, '|');

    if (command != "UPLOAD" || username.empty() || filename.empty() || fileSizeStr.empty()) {
        send(clientSocket, " Invalid upload request", 25, 0);
        return false;
    }

    size_t fileSize = std::stoull(fileSizeStr);

    std::filesystem::create_directories("data/" + username);
    std::string filePath = "data/" + username + "/" + filename;

    // Acknowledge the header
    send(clientSocket, "READY", 5, 0);

    // Now receive the file
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile) return false;

    char buffer[4096];
    size_t totalReceived = 0;

    while (totalReceived < fileSize) {
        int chunk = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (chunk <= 0) break;
        outFile.write(buffer, chunk);
        totalReceived += chunk;
    }

    outFile.close();

    if (totalReceived == fileSize) {
        send(clientSocket, " Upload successful", 22, 0);
        return true;
    }
    else {
        send(clientSocket, " Upload incomplete", 21, 0);
        return false;
    }
}


bool UploadHandler::startUpload(int clientSocket, const std::string& username, const std::string& filename) {
    std::filesystem::create_directories("data/" + username);
    std::string filePath = "data/" + username + "/" + filename;

    bool success = saveFile(clientSocket, filePath);
    if (!success || wasInterrupted()) {
        send(clientSocket, "Upload interrupted. Please try again.", 40, 0);
        return false;
    }

    send(clientSocket, "Upload successful", 18, 0);
    return true;
}

bool UploadHandler::wasInterrupted() const {
    return interrupted;
}