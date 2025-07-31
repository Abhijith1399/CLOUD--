#ifndef SECURITY_CHECK_H
#define SECURITY_CHECK_H

#include<iostream>
#include <string>
#include <unordered_set>
#include <mutex>

using namespace std;

class SecurityCheck {

private:

    static unordered_set<string> loggedInUsers;
    static mutex userMutex;


public:
    static void loginUser(const string& username);
    static void logoutUser(const string& username);
    static bool isLoggedIn(const string& username);
};

#endif
