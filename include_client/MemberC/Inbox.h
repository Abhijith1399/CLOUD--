#ifndef INBOX_H
#define INBOX_H

#include<iostream>
#include<string>
#include<thread>
#include<mutex>

using namespace std;

class Inbox {
private:
	string username;
	mutex inboxMutex;

public:
	void setUsername(string username);
	string getUsername();

	void listInbox();                     // uses class variable
	void listInbox(const string& username);  // uses direct value
};

#endif
