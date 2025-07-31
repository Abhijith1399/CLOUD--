#pragma once
#include <string>

class UploadHandler
{
private:
    bool interrupted;
    bool saveFile(int clientSocket, const std::string& path);

public:
    UploadHandler();

    bool receiveFile(int clientSocket, const std::string& username);
    bool startUpload(int clientSocket, const std::string& username, const std::string& filename);
    bool wasInterrupted() const;
};