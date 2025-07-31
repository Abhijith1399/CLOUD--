#include "MemberB/UplaodLogger.h"
#include<iostream>
#include <fstream>
#include <ctime>

void UploadLogger::logUpload(const std::string& username, const std::string& filename)
{
    std::ofstream log("logs/upload_logs.txt", std::ios::app);
    std::time_t now = std::time(nullptr);
    char timeBuffer[26]; // ctime_s requires a buffer of at least 26 chars

    if (ctime_s(timeBuffer, sizeof(timeBuffer), &now) != 0) {
        log << username << " uploaded " << filename << " at " << "UnknownTime\n";
    }
    else {
        log << username << " uploaded " << filename << " at " << timeBuffer;
    }
}