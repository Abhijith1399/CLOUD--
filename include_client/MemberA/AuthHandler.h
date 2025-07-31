#pragma once
#ifndef AUTH_HANDLER_H
#define AUTH_HANDLER_H

#include <string>
#include <vector>

class AuthHandler
{
public:
    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
private:
    std::string userFile = "data/users.txt";
    bool userExists(const std::string& username);
};

#endif