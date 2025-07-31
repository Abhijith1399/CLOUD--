#ifndef DELETEHANDLER_H
#define DELETEHANDLER_H

#include <string>
using namespace std;

class DeleteHandler {
private:
    string username;
    string filename;

public:
    void setUsername(string username);
    void setFilename(string filename);
    string getUsername();
    string getFilename();

    void deleteFile(const string& username, const string& filename);
};

#endif
