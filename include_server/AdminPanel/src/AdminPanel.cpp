#include "../include/AdminPanel/AdminBase.h"
#include "../include/AdminPanel/AdminPanel.h"
#include<iostream>
#include<thread>
#include<mutex>
#include<fstream>
#include <string>
using namespace std;

AdminPanel::AdminPanel() : AdminBase() {}
void AdminPanel::showDashboard() {
  cout << "Admin Dashboard"<<endl;
    cout << "1. View Reports"<<endl;
    cout << "2.Clean Trash " << endl;
    cout << "3. Storage " << endl;
    int ch;
    cin >> ch;
    switch (ch) {
    case 1: listLogs(); break;
    case 2: approveRequests(); break;
    default:
        cout << "Invalid."<<endl;
    }
}
void AdminPanel::listLogs() {
   lock_guard<mutex> lock(panelLock);
   ifstream log("logs/report.txt");
   string line;
   cout << "Logs:"<<endl;
    while (getline(log, line)) {
       cout << line << "\n";
    }
}
void AdminPanel::approveRequests() {
    lock_guard<std::mutex> lock(panelLock);
    ifstream req("Database/upgrade_requests.txt");
    ofstream out("Database/approved.txt", ios::app);
    string user;
    while (getline(req, user)) {
        char c;
        cout << "Approve " << user << "? (y/n): ";
        cin >> c;
        if (c == 'y') {
            out << user << "\n";
        }
    }
}
AdminBase::AdminBase() {
    adminUsername = "admin";
    adminPassword = "admin123";
}
bool AdminBase::validate(const string& u, const string& p) {
    return u == adminUsername && p == adminPassword;
}