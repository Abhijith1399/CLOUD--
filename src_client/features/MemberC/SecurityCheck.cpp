#include<iostream>
#include "MemberC/SecurityCheck.h"

using namespace std;

unordered_set<string> SecurityCheck::loggedInUsers;

mutex SecurityCheck::userMutex;

void SecurityCheck::loginUser(const string& username) {

    lock_guard<mutex> lock(userMutex);

    loggedInUsers.insert(username);
}

void SecurityCheck::logoutUser(const string& username) {

    lock_guard<mutex> lock(userMutex);

    loggedInUsers.erase(username);
}

bool SecurityCheck::isLoggedIn(const string& username) {

    lock_guard<mutex> lock(userMutex);

    return loggedInUsers.find(username) != loggedInUsers.end();
}
