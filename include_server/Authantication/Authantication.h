
#pragma once
#include <string>

class Authentication {
public:
    //register users data store in data/users.txt and also store the login data
    Authentication(const std::string& userFile = "data/users.txt",
        const std::string& loginLogFile = "data/logs/loginlog.txt");
    //register user function_____________________________________
    bool registerUser(const std::string& fullname,
        const std::string& username,
        const std::string& password,
        const std::string& email,
        const std::string& phone,
        const std::string& address);
    //login user function_________________________________________
    bool loginUser(const std::string& username, const std::string& password);

private:
    const std::string m_userFile;
    const std::string m_loginLogFile;
    //store login log
    void logLogin(const std::string& username, bool success);
    bool userExists(const std::string& username);//function for avoid multiple user same name
    std::string hashPassword(const std::string& password);//password hashing
};

