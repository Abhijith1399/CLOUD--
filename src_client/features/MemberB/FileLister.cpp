#include "MemberB/FileLister.h"
#include <filesystem>
#include<iostream>

namespace fs = std::filesystem;

FileLister::FileLister() {
    baseDirectory = "server_storage";
}

void FileLister::setBaseDirectory(const std::string& path) {
    baseDirectory = path;
}

std::string FileLister::getBaseDirectory() const {
    return baseDirectory;
}

std::vector<std::string> FileLister::listFilesFromFolder(const std::string& folderPath) const {
    std::vector<std::string> files;
    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        // Folder doesn't exist or inaccessible
    }
    return files;
}

std::vector<std::string> FileLister::listUserFiles(const std::string& username) const {
    std::string userPath = baseDirectory + "/" + username;
    return listFilesFromFolder(userPath);
}