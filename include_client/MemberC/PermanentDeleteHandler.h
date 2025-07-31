#ifndef PERMANENT_DELETE_HANDLER_H
#define PERMANENT_DELETE_HANDLER_H

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "base/TrashBase.h"

using namespace std;

class PermanentDeleteHandler : public TrashBase {

private:
    string username;
    string filename;
    mutex perMutex;

public:
    void setUsername(string username);
    string getUsername();

    void setFilename(string filename);
    string getFilename();

    void deletePermanently();// Uses internal variables
    void deletePermanently(const string& username, const string& filename);  // Sets values and calls above
};

#endif

