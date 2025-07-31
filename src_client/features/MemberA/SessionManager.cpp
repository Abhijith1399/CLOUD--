
#include "MemberA/SessionManager.h"
#include "../features/NetworkUtils.h"  // Required for sendToServer
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;


const string sessionFile = "data/session.txt";  

void SessionManager::createSession(const string& username)
{
    ofstream file(sessionFile, ios::app);
    file << username << "\n";
}

void SessionManager::endSession(const string& username)
{
  
    string request = "LOGOUT|" + username;
    string response;

    if (sendToServer(request, response)) {
        cout << "Logout sent to server. Server Response: " << response << "\n";
    }
    else {
        cout << "Failed to send logout to server.\n";
    }

    
    ifstream infile(sessionFile);
    vector<string> users;
    string line;

    while (getline(infile, line)) {
        if (line != username)
            users.push_back(line);
    }
    infile.close();

    ofstream outfile(sessionFile);
    for (const auto& u : users)
        outfile << u << "\n";
}

bool SessionManager::isUserLoggedIn(const string& username)
{
    ifstream file(sessionFile);
    string line;
    while (getline(file, line)) {
        if (line == username)
            return true;
    }
    return false;
}
