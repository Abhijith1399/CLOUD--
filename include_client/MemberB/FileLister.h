#pragma once
#include <vector>
#include <string>

class FileLister {
private:
    std::string baseDirectory;

    std::vector<std::string> listFilesFromFolder(const std::string& folderPath) const;

public:
    FileLister();
    void setBaseDirectory(const std::string& path);
    std::string getBaseDirectory() const;

    std::vector<std::string> listUserFiles(const std::string& username) const;
};