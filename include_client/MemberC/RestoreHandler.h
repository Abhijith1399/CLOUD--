#ifndef RESTORE_HANDLER_H
#define RESTORE_HANDLER_H

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "base/TrashBase.h"

using namespace std;

class RestoreHandler : public TrashBase {
private:
    string username;
    string filename;
    mutex restoreMutex;

public:
    void setUsername(string username);
    string getUsername();

    void setFilename(string filename);
    string getFilename();

    void restoreFile();  // uses class members
    void restoreFile(const string& username, const string& filename);  // uses direct input
};

#endif

