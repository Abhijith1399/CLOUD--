#ifndef ADMINPANEL_H
#define ADMINPANEL_H
#include "AdminBase.h"
#include<iostream>
#include<mutex>
#include<thread>
using  namespace std;
class AdminPanel : public AdminBase {
private:
	mutex panelLock;
public:
	AdminPanel();
	void showDashboard() override;
	void listLogs();
	void approveRequests();
};
#endif