#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>

#include "../include/Authantication/Authantication.h"
#include "../include/StorageMonitor/StorageMonitor.h"

#pragma comment(lib, "Ws2_32.lib")

namespace fs = std::filesystem;

constexpr int SERVER_PORT = 8080;
constexpr int BUFFER_SIZE = 4096;

static Authentication auth;
static StorageMonitor quota("data/quotas.txt", 500);//total cloud storage is 500mb
static std::mutex coutMux;
static std::mutex logMux;

size_t getFolderSize(const fs::path& folder) {
    size_t total_size = 0;
    if (!fs::exists(folder)) return 0;

    for (const auto& entry : fs::recursive_directory_iterator(folder)) {
        if (fs::is_regular_file(entry)) {
            total_size += fs::file_size(entry);
        }
    }
    return total_size;
}

void safePrint(const std::string& msg) {
    std::lock_guard<std::mutex> lock(coutMux);
    std::cout << msg << std::endl;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \r\n");
    size_t end = str.find_last_not_of(" \r\n");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// current time and date getting
std::string currentDateTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t nowTimeT = std::chrono::system_clock::to_time_t(now);
    std::tm nowTm;
    localtime_s(&nowTm, &nowTimeT);

    std::ostringstream oss;
    oss << std::put_time(&nowTm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// here to strore all actions like server clientnresponse are store in report.txt
void serverLog(const std::string& entry) {
    std::lock_guard<std::mutex> lock(logMux);
    std::ofstream logFile("data/logs/report.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" << currentDateTime() << "] " << entry << std::endl;
        logFile.close();
    }
}

// Sends entire message reliably on socket
bool sendAll(SOCKET sock, const std::string& msg) {
    int totalSent = 0;
    int msgLen = (int)msg.size();
    while (totalSent < msgLen) {
        int sent = send(sock, msg.c_str() + totalSent, msgLen - totalSent, 0);
        if (sent == SOCKET_ERROR) return false;
        totalSent += sent;
    }
    return true;
}

void clientHandler(SOCKET sock) {
    char buffer[BUFFER_SIZE];

    safePrint("[Client sucess]");

    while (true) {
        int recvLen = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (recvLen <= 0) {
            safePrint("[Client sucess]");
            break;
        }

        buffer[recvLen] = '\0';
        std::string request = trim(std::string(buffer));

        safePrint("Received: " + request);

        std::istringstream iss(request);
        std::string cmd;
        if (!std::getline(iss, cmd, '|')) {
            safePrint("Failed to parse command");
            break;
        }
        //----------------------REGISTER--------------------------------------
        if (cmd == "REGISTER") {
            std::string fullname, username, password, email, phone, address;
            std::getline(iss, fullname, '|');
            std::getline(iss, username, '|');
            std::getline(iss, password, '|');
            std::getline(iss, email, '|');
            std::getline(iss, phone, '|');
            std::getline(iss, address, '|');

            bool ok = auth.registerUser(fullname, username, password, email, phone, address);
            if (ok) quota.addUser(username);
            sendAll(sock, ok ? "REGISTER_SUCCESS" : "REGISTER_FAILED");

            serverLog("REGISTER: User=" + username + " Result=" + (ok ? "SUCCESS" : "FAILED"));
        }
        //----------------------LOGIN--------------------------------------
        else if (cmd == "LOGIN") {
            std::string username, password;
            std::getline(iss, username, '|');
            std::getline(iss, password, '|');

            bool ok = auth.loginUser(username, password);
            sendAll(sock, ok ? "LOGIN_SUCCESS" : "LOGIN_FAILED");

            serverLog("LOGIN: User=" + username + " Result=" + (ok ? "SUCCESS" : "FAILED"));
        }
     

        //----------------------UPLOAD---------------------------------------
        else if (cmd == "UPLOAD") {
            std::string username, filename, sizeStr;
            std::getline(iss, username, '|');
            std::getline(iss, filename, '|');
            std::getline(iss, sizeStr, '|');

            size_t filesize = 0;
            try {
                filesize = std::stoull(sizeStr);
            }
            catch (...) {
                sendAll(sock, "UPLOAD_FAILED_INVALID_SIZE");
                continue;
            }

            if (!quota.canUpload(username, filesize)) {
                sendAll(sock, "UPLOAD_FAILED_QUOTA_EXCEEDED");
                continue;
            }

            if (!sendAll(sock, "READY")) {
                safePrint("Failed to send READY");
                break;
            }

            fs::path userPath = fs::path("storage") / username;
            fs::create_directories(userPath);
            fs::path filePath = userPath / filename;

            std::ofstream ofs(filePath, std::ios::binary);
            if (!ofs) {
                sendAll(sock, "UPLOAD_FAILED_FILE_WRITE");
                continue;
            }

            size_t totalReceived = 0;
            while (totalReceived < filesize) {
                size_t toRead = min(filesize - totalReceived, sizeof(buffer));
                int bytesRead = recv(sock, buffer, static_cast<int>(toRead), 0);
                if (bytesRead <= 0) {
                    safePrint("Upload interrupted");
                    break;
                }
                ofs.write(buffer, bytesRead);
                totalReceived += bytesRead;
            }
            ofs.close();

            if (totalReceived == filesize && quota.updateQuotaOnUpload(username, filesize)) {
                sendAll(sock, "UPLOAD_SUCCESS");

                // Optional logging of upload
                std::ofstream uplog("data/logs/upload.log", std::ios::app);
                uplog << username << "|" << filename << "|" << filesize << "\n";

                safePrint("Upload complete: " + username + "/" + filename);
            }
            else {
                if (fs::exists(filePath))
                    fs::remove(filePath);
                sendAll(sock, "UPLOAD_FAILED");
                safePrint("Upload failed: " + username + "/" + filename);
            }
        }
        //----------------------  DOWLOAD---------------------------------------
        else if (cmd == "DOWNLOAD") {
            std::string username, filename;
            std::getline(iss, username, '|');
            std::getline(iss, filename, '|');

            fs::path filePath = fs::path("storage") / username / filename;
            if (!fs::exists(filePath) || !fs::is_regular_file(filePath)) {
                sendAll(sock, "DOWNLOAD_FAILED_FILE_NOT_FOUND");
                continue;
            }

            size_t fileSize = fs::file_size(filePath);
                     
 
            std::string fileHeader = "FILE|" + filename + "|" + std::to_string(fileSize) + "|";
            if (!sendAll(sock, fileHeader)) {
                safePrint("Failed to send file header to client");
                break;
            }

            std::ifstream ifs(filePath, std::ios::binary);
            if (!ifs) {
                safePrint("Failed to open file for reading: " + filePath.string());
                break;
            }

            size_t totalSent = 0;
            while (totalSent < fileSize) {
                ifs.read(buffer, sizeof(buffer));
                std::streamsize bytesRead = ifs.gcount();
                if (bytesRead <= 0)
                    break;

                int bytesSent = send(sock, buffer, static_cast<int>(bytesRead), 0);
                if (bytesSent == SOCKET_ERROR) {
                    safePrint("Failed to send file data");
                    break;
                }
                totalSent += bytesSent;
            }
            ifs.close();

            if (totalSent < fileSize) {
                safePrint("File transfer incomplete: " + filename);
            }
            else {
                safePrint("File transfer complete: " + filename);
            }
        }

        //---------------------- PERMANANT DELETE--------------------------------------


        else if (cmd == "TRASH")
        {
            std::string username, filename;
            getline(iss, username, '|');
            getline(iss, filename, '|');

            fs::path userTrashDir = fs::path("storage") / username / "trash";
            fs::path permDeletedRoot = fs::path("Trash") / "PermanentDeleted";
            fs::path userPermDeletedDir = permDeletedRoot / username / "trash";

            fs::path sourceFile = userTrashDir / filename;
            fs::path targetFile = userPermDeletedDir / filename;

            if (!fs::exists(sourceFile) || !fs::is_regular_file(sourceFile))
            {
                sendAll(sock, "PERMANENT_DELETE_FAILED_NOT_FOUND");
                continue;
            }

            // Create permanent deleted root and user's folder (with trash) if needed
            std::error_code ec;
            if (!fs::exists(permDeletedRoot))
            {
                fs::create_directories(permDeletedRoot, ec);
                if (ec)
                {
                    sendAll(sock, "PERMANENT_DELETE_FAILED_INTERNAL");
                    continue;
                }
            }
            if (!fs::exists(userPermDeletedDir))
            {
                fs::create_directories(userPermDeletedDir, ec);
                if (ec)
                {
                    sendAll(sock, "PERMANENT_DELETE_FAILED_INTERNAL");
                    continue;
                }
            }

            // Enforce total size limit (example: 50 MB)
            const size_t kMaxPermDeletedSize = 50ull * 1024 * 1024;

            size_t fileSize = fs::file_size(sourceFile);
            size_t currentPermDeletedSize = getFolderSize(permDeletedRoot);

            if (currentPermDeletedSize + fileSize > kMaxPermDeletedSize)
            {
                sendAll(sock, "PERMANENT_DELETE_FAILED_SPACE_LIMIT");
                continue;
            }

            // Move file from user's trash to permanent deleted folder
            fs::rename(sourceFile, targetFile, ec);
            if (ec)
            {
                sendAll(sock, "PERMANENT_DELETE_FAILED_MOVE");
                safePrint("Error moving file to PermanentDeleted: " + ec.message());
                continue;
            }

            // Decrease user's quota accordingly
            if (!quota.updateQuotaOnDelete(username, fileSize))
            {
                sendAll(sock, "PERMANENT_DELETE_FAILED_QUOTA_UPDATE");
                // Optionally log this non-fatal error
            }

            sendAll(sock, "PERMANENT_DELETE_SUCCESS");
            safePrint("User " + username + " permanently deleted file: " + filename);
        }

        //---------------------- SHARE TO SHARE--------------------------------------

        else if (cmd == "SHARE") {
            std::string sender, receiver, filename;
            getline(iss, sender, '|');
            getline(iss, receiver, '|');
            getline(iss, filename, '|');

            // Check if sender owns the file
            fs::path senderFile = fs::path("storage") / sender / filename;
            if (!fs::exists(senderFile) || !fs::is_regular_file(senderFile)) {
                sendAll(sock, "SHARE_FAILED_FILE_NOT_FOUND");
                continue;
            }

            // Check if receiver exists (you can add method in Authentication)
           /* if (!auth.userExists(receiver)) {
                sendAll(sock, "SHARE_FAILED_RECEIVER_NOT_FOUND");
                continue;
            }*/

            // Ensure inbox folder exists for receiver
            fs::path receiverInbox = fs::path("storage") / receiver / "inbox";
            if (!fs::exists(receiverInbox)) {
                std::error_code ec;
                fs::create_directories(receiverInbox, ec);
                if (ec) {
                    sendAll(sock, "SHARE_FAILED_CREATE_INBOX");
                    continue;
                }
            }

            // Check receiver quota
            size_t fileSize = fs::file_size(senderFile);
            if (!quota.canUpload(receiver, fileSize)) {
                sendAll(sock, "SHARE_FAILED_QUOTA_EXCEEDED");
                continue;
            }

            fs::path receiverFile = receiverInbox / filename;

            // Copy file to receiver inbox
            std::error_code ec;
            fs::copy_file(senderFile, receiverFile, fs::copy_options::overwrite_existing, ec);
            if (ec) {
                sendAll(sock, "SHARE_FAILED_COPY_ERROR");
                continue;
            }

            // Update receiver quota (increase)
            if (!quota.updateQuotaOnUpload(receiver, fileSize)) {
                // Rollback copy on quota update failure
                fs::remove(receiverFile);
                sendAll(sock, "SHARE_FAILED_QUOTA_UPDATE");
                continue;
            }

            sendAll(sock, "SHARE_SUCCESS");
            safePrint("File shared from '" + sender + "' to '" + receiver + "': " + filename);
        }



        //---------------------- LIST--------------------------------------

        else if (cmd == "LIST") {
            std::string username;
            std::getline(iss, username, '|');

            fs::path userDir = fs::path("storage") / username;
            std::string fileList;

            if (fs::exists(userDir)) {
                for (const auto& entry : fs::directory_iterator(userDir)) {
                    if (fs::is_regular_file(entry))
                        fileList += entry.path().filename().string() + "\n";
                }
            }

            sendAll(sock, fileList.empty() ? "NO_FILES" : fileList);
        }


        //----------------------  DELETE---------------------------------------
        else if (cmd == "DELETE") {
            std::string username, filename;
            getline(iss, username, '|');
            getline(iss, filename, '|');

            fs::path userFolder = fs::path("storage") / username;
            fs::path trashFolder = userFolder / "trash";
            fs::path filePath = userFolder / filename;
            fs::path trashFilePath = trashFolder / filename;

            if (!fs::exists(filePath) || !fs::is_regular_file(filePath)) {
                sendAll(sock, "DELETE_FAILED_NOT_FOUND");
                continue;
            }

            // Create trash folder if not exists
            if (!fs::exists(trashFolder)) {
                fs::create_directories(trashFolder);
            }

            // Move file to trash: rename/move instead of copy+delete
            std::error_code ec;
            fs::rename(filePath, trashFilePath, ec);
            if (ec) {
                sendAll(sock, "DELETE_FAILED");
                safePrint("Failed to move file to trash: " + ec.message());
                continue;
            }

            // Update quota file to remove deleted file size
            size_t fileSize = fs::file_size(trashFilePath);

            if (quota.updateQuotaOnDelete(username, fileSize)) {
                sendAll(sock, "DELETE_SUCCESS");
                safePrint("Moved file to trash and updated quota: " + trashFilePath.string());
            }
            else {
                sendAll(sock, "DELETE_QUOTA_UPDATE_FAILED");
            }
        }
        //------------------ RESTORE -----------------------------------------

        else if (cmd == "RESTORE") {
            std::string username, filename;
            getline(iss, username, '|');
            getline(iss, filename, '|');

            fs::path userDir = fs::path("storage") / username;
            fs::path trashDir = userDir / "trash";
            fs::path trashFile = trashDir / filename;
            fs::path restoredFile = userDir / filename;

            if (!fs::exists(trashFile) || !fs::is_regular_file(trashFile)) {
                sendAll(sock, "RESTORE_FAILED_NOT_FOUND");
                continue;
            }

            // Check if quota allows adding file back
            size_t fileSize = fs::file_size(trashFile);

            if (!quota.canUpload(username, fileSize)) {
                sendAll(sock, "RESTORE_FAILED_QUOTA_EXCEEDED");
                continue;
            }

            // Move file back from trash to storage
            std::error_code ec;
            fs::rename(trashFile, restoredFile, ec);
            if (ec) {
                sendAll(sock, "RESTORE_FAILED");
                safePrint("Restore failed: " + ec.message());
                continue;
            }

            // Update quota usage
            if (!quota.updateQuotaOnRestore(username, fileSize)) {
                sendAll(sock, "RESTORE_FAILED_QUOTA_UPDATE");
                // Optionally, move file back to trash or handle rollback
                continue;
            }

            sendAll(sock, "RESTORE_SUCCESS");
            safePrint("File restored for user " + username + ": " + filename);
        }


        //_________________  LOGOUT_______________________________

        else if (cmd == "LOGOUT") {
            sendAll(sock, "LOGOUT_SUCCESS");
            break;  // close connection
        }



        else {
            sendAll(sock, "UNKNOWN_COMMAND");
            serverLog("UNKNOWN_COMMAND: " + cmd);
        }
    }

    closesocket(sock);
    serverLog("Client closed");
}

int main() {//windows socket adress for understand to os 
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {// wsastartup librarary sto net work on  
        std::cerr << "WSAStartup failed\n";// makeword is ued to version library of windows network 2.2
        return 1;
    }
    //socket creation for listenSock
    // AF_INET adress famiy internet  
    //sock stream in tcp  
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }
    //--binding___________________________
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);//port address
    serverAddr.sin_addr.s_addr = INADDR_ANY;// INADDR_ANY   reciever adress
    //binding the server adrees and port 
    if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
//-------------listen-----------------------------------
    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR)  //  server is listening the client.....
    {    //SOMAXCONN   is used so maximum  connection in queue
        std::cerr << "Listen failed\n";
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    safePrint("Server started...");
    //server accepttong and build the connection
    while (true) {
        SOCKET clientSock = accept(listenSock, nullptr, nullptr);
        if (clientSock == INVALID_SOCKET) {
            safePrint("Accept failed");
            continue;
        }
        // creating thread for individual client ,client handler used to take all comment to server
        std::thread(clientHandler, clientSock).detach();
    }

    closesocket(listenSock);
    WSACleanup();
    return 0;
}