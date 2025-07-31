#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <string>

class ConnectionManager {
public:
    int connectToServer(const std::string& ip, int port);
};

#endif
