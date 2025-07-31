#pragma once
#ifndef LOGREADER_H
#define LOGREADER_H

#include <string>

class LogReader {
private:
    std::string basePath;

public:
    //login actions are store in data/logs
    LogReader(const std::string& basePath = "data/logs/");
    //show what are action taken by user
    void showUserActivity(const std::string& username);
};

#endif // LOGREADER_H
