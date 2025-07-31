#ifndef USERVALIDATOR_H
#define USERVALIDATOR_H

#include <string>

class UserValidator {
public:
    static bool isValidUsername(const std::string& username);
    static bool isValidPassword(const std::string& password);
    static bool isValidEmail(const std::string& email);
    static bool isValidPhone(const std::string& phone);
};

#endif