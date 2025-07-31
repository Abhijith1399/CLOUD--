#pragma once
#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <string>

class SessionManager {
public:
    void createSession(const std::string& username);
    void endSession(const std::string& username);
    bool isUserLoggedIn(const std::string& username);
private:
    std::string sessionFile = "data/sessions.txt";
};

#endif