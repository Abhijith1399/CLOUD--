#ifndef CLIENT_DOWNLOAD_H
#define CLIENT_DOWNLOAD_H

#include <string>
#include <winsock2.h>

// Function to receive a file from server and save it in local /downloads/ folder
void downloadFileFromServer(SOCKET sock, const std::string& username, const std::string& filename);

#endif  // CLIENT_DOWNLOAD_H
