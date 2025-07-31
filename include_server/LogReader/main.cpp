#include "LogReader.h"
#include <iostream>

int main() {
    std::string username;
    std::cout << "Enter username to view activity log: ";
    std::cin >> username;

    LogReader reader;
    reader.showUserActivity(username);

    return 0;
}
