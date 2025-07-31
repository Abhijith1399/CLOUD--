#include "MemberC/Inbox.h"
#include <iostream>
#include <filesystem>

using namespace std;

void Inbox::setUsername(string username) {
	this->username = username;
}

string Inbox::getUsername() {
	return this->username;
}

void Inbox::listInbox() {
	thread([=]() {
		lock_guard<mutex> lock(inboxMutex);

		string inboxPath = "storage/" + username + "/inbox/";

		cout << "Checking inbox for user: " << username << endl;

		if (!filesystem::exists(inboxPath)) {
			cout << "Inbox folder not found." << endl;
			return;
		}

		bool empty = true;

		for (const auto& entry : filesystem::directory_iterator(inboxPath)) {
			cout << entry.path().filename().string() << endl;
			empty = false;
		}

		if (empty) {
			cout << "Inbox is empty." << endl;
		}
		}).detach();
}

void Inbox::listInbox(const string& username) {
	setUsername(username);
	listInbox();  // reuse above
}
