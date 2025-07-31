#ifndef SHARE_BASE_H
#define SHARE_BASE_H

#include<iostream>
#include <string>

using namespace std;

class ShareBase {
public:
    virtual bool validateReceiver(const string& receiver);
};

#endif
