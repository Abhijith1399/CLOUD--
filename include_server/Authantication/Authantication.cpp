
#include "../include/Authantication/Authantication.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <mutex>

namespace fs = std::filesystem;
static std::mutex logMutex;
//Create a file path of new user_______________________________
Authentication::Authentication(const std::string& userFile,
    const std::string& loginLogFile)
    : m_userFile(userFile),
    m_loginLogFile(loginLogFile)
{
    fs::create_directories(fs::path(m_userFile).parent_path());
    fs::create_directories(fs::path(m_loginLogFile).parent_path());
}
//time and data recoding use full log record___________________________________
static std::string nowTimestamp() {
    auto now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
//to store password in txt file like a humanunreadable_____________________________
std::string Authentication::hashPassword(const std::string& password) {
    std::string hashed = password;
    for (char& c : hashed) c = c + 1;
    return hashed;
}
//Autantication of username_____________________________________________
bool Authentication::userExists(const std::string& username) {
    std::ifstream fin(m_userFile);
    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::string fullname, user;
        if (std::getline(iss, fullname, '|') && std::getline(iss, user, '|')) {
            if (user == username) return true;
        }
    }
    return false;
}
//Autantication for register_________________________________________________
bool Authentication::registerUser(const std::string& fullname,const std::string& username, const std::string& password,
    const std::string& email,const std::string& phone,const std::string& address)
{
    if (userExists(username)) return false;
    std::ofstream fout(m_userFile, std::ios::app);
    if (!fout) return false;
    fout << fullname << '|' << username << '|' << hashPassword(password)
        << '|' << email << '|' << phone << '|' << address << '\n';
    return true;
}
//Authantication for login credential________________________________________________
void Authentication::logLogin(const std::string& username, bool success) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream log(m_loginLogFile, std::ios::app);
    log << "[" << nowTimestamp() << "] Username: " << username
        << " - Login " << (success ? "Success" : "Failure") << "\n";
}

//Authantication for login user ________________________________________________
bool Authentication::loginUser(const std::string& username, const std::string& password) {
    std::ifstream fin(m_userFile);
    std::string hashed = hashPassword(password), line;
    bool ok = false;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::string fullname, user, pass;
        if (std::getline(iss, fullname, '|') &&
            std::getline(iss, user, '|') &&
            std::getline(iss, pass, '|') &&
            user == username && pass == hashed) {
            ok = true;
            break;
        }
    }
    logLogin(username, ok);
    return ok;
}


