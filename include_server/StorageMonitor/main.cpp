#include <iostream>
#include <vector>
#include <string>
#include "StorageMonitor.h"

int main() {
    StorageMonitor monitor;

    // Optionally, show all users first
    std::cout << "Current Storage Usage for all users:\n";
    monitor.showAll();

    // Ask admin to input username
    std::cout << "\nEnter username to check quota details: ";
    std::string username;
    std::getline(std::cin, username);

    // Show details for that user
    std::string result = monitor.showUserUsage(username);
    std::cout << result << std::endl;

    return 0;
}
