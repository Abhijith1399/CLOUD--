#include "MemberA/AuthHandler.h"
#include <fstream>
#include <sstream>
#include<iostream>

using namespace std;

bool AuthHandler::userExists(const string& username)
{
    ifstream file(userFile);
    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string user, pass;
        ss >> user >> pass;
        if (user == username) return true;
    }
    return false;
}


bool AuthHandler::registerUser(const string& username, const string& password)
{
    if (userExists(username)) return false;

    ofstream file(userFile, ios::app);
    file << username << " " << password << "\n";
    return true;
}

bool AuthHandler::loginUser(const string& username, const string& password)
{
    ifstream file(userFile);
    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string user, pass;
        ss >> user >> pass;
        if (user == username && pass == password) return true;
    }
    return false;
}

