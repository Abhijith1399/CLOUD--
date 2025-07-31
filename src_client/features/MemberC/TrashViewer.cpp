#include "MemberC/TrashViewer.h"
#include <filesystem>
#include <iostream>

using namespace std;


void TrashViewer::setUsername(string username) {
    this->username = username;
}


string TrashViewer::getUsername() {
    return this->username;
}


void TrashViewer::listTrash() {
    thread([=]() {
        lock_guard<mutex> lock(trashMutex);

        string trashPath = "storage/" + username + "/trash/";

        cout << "Viewing trash for user: " << username << endl;

        if (!filesystem::exists(trashPath)) {
            cout << "Trash folder does not exist.\n";
            return;
        }

        bool empty = true;
        for (const auto& entry : filesystem::directory_iterator(trashPath)) {
            cout << entry.path().filename().string() << endl;
            empty = false;
        }

        if (empty) {
            cout << "Trash is empty.\n";
        }
        }).detach();
}

// Overloaded version if username is passed directly
void TrashViewer::listTrash(const string& username) {
    setUsername(username);
    listTrash();
}

