#define NOMINMAX
#define WIN32_LEAN_AND_MEAN//it used to prevent rarely used windows header
#undef byte
#include <iostream>
#include "MemberC/DeleteHandler.h"
#include "../features/NetworkUtils.h"  
#include <filesystem>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;
namespace fs = std::filesystem;

void DeleteHandler::deleteFile(const string& username, const string& filename) {
    string request = "DELETE|" + username + "|" + filename;
    string response;

    cout << " Sending delete request to server...\n";

    if (sendToServer(request, response)) {
        cout << " Server Response: " << response << "\n";

        string clientUserDir = "storage/" + username + "/";//storage folder
        string clientSrc = clientUserDir + filename;//file folder
        string clientTrash = clientUserDir + "trash/";//trash folder

        try {
            fs::create_directories(clientTrash);

            if (fs::exists(clientSrc)) {
                fs::rename(clientSrc, clientTrash + filename);
                cout << " Local file moved to client trash.\n";
            }
            else {
                cout << " Local file not found: " << clientSrc << "\n";
            }
        }
        catch (const exception& e) {
            cerr << " Local trash error: " << e.what() << "\n";
        }

    }
    else {
        cout << " Failed to send delete request to server.\n";
    }
}
