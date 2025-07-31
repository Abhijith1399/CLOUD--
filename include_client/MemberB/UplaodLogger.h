#pragma once
#include <string>

class UploadLogger
{
public:
    static void logUpload(const std::string& username, const std::string& filename);
};