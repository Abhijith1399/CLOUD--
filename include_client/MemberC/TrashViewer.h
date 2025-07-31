#ifndef TRASH_VIEWER_H
#define TRASH_VIEWER_H

#include <iostream>
#include <string>
#include <thread>
#include <mutex>

using namespace std;

class TrashViewer {
private:
    string username;
    mutex trashMutex;

public:
    // Setter for username
    void setUsername(string username);

    // Getter for username
    string getUsername();

    // List trash using internal username variable
    void listTrash();

    // Overloaded version (optional)
    void listTrash(const string& username);
};

#endif
