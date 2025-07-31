#pragma once
#ifndef ADMINBASE_H
#define ADMINBASE_H
#include <iostream>
using namespace std;
class AdminBase {
protected:
	string adminUsername;
	string adminPassword;
public:
	AdminBase();
	virtual bool validate(const string& u, const string& p);
	virtual void showDashboard() = 0;
	virtual ~AdminBase() {}
};
#endif