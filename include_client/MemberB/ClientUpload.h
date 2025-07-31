#ifndef CLIENT_UPLOAD_H
#define CLIENT_UPLOAD_H

#include <string>
#include <winsock2.h>

std::string selectFileDialog();  // Open file dialog to choose a local file

bool uploadFileToServer(SOCKET clientSock, const std::string& username);  // Upload selected file to server

#endif // CLIENT_UPLOAD_H
