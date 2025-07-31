#ifndef TRASH_BASE_H
#define TRASH_BASE_H


#include<iostream>
#include <string>

using namespace std;

class TrashBase {

public:
    virtual void logTrashAction(const string& user, const string& action);
};

#endif
