#pragma once
#include <string>

class DownloadHandler
{
private:
    bool interrupted;
    bool sendFileContent(int clientSocket, const std::string& path);
public:
    DownloadHandler();
    bool sendFile(int clientSocket, const std::string& username, const std::string& filename);
    bool wasInterrupted() const;
};