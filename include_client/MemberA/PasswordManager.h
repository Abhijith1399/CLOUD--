#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <string>

class PasswordManager {
public:
    static std::string hashPassword(const std::string& password);
};

#endif

