#include "../include/AdminPanel/AdminPanel.h"
#include <iostream>
#include <string>

int main() {
    string username, password;

   cout << "Cloud++ Admin Portal"<<endl;
  cout << "Username: ";
   cin >> username;

   cout << "Password: ";
    cin >> password;

    AdminPanel admin;

    if (admin.validate(username, password)) {
        cout << "Login successful"<<endl;
        admin.showDashboard();
    }
    else {
        cout << "Access denied" << endl;
    }

    return 0;
}
