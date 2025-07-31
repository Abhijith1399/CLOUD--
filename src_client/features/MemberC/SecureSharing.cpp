
#include "MemberC/SecureSharing.h"
#include "MemberC/SecurityCheck.h"
#include "NetworkUtils.h"
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

void SecureSharing::setSender(const string s) {
    this->sender = s;
}

string SecureSharing::getSender()  {
    return this->sender;
}

void SecureSharing::setReceiver(const string r) {
    this->receiver = r;
}

string SecureSharing::getReceiver()  {
    return this->receiver;
}

void SecureSharing::setFilename(const string f) {
    this->filename = f;
}

string SecureSharing::getFilename()  {
    return this->filename;
}

bool SecureSharing::shareFile() {
    
    if (!SecurityCheck::isLoggedIn(sender)) {
        cout << "Sender not logged in.\n";
        return false;
    }
    string request = "SHARE|" + sender + "|" + receiver + "|" + filename + "|";
    string response;

    cout << "Sending SHARE request to server...\n";
    if (!sendToServer(request, response)) {
        cout << "Could not connect to server.\n";
        return false;
    }

    cout << "Server Response: " << response << "\n";

    
    if (response.compare(0, 13, "SHARE_SUCCESS") == 0) {
        
        fs::path senderLocalFile = fs::path("storage") / sender / filename;
        fs::path receiverInboxDir = fs::path("storage") / receiver / "inbox";

        try {
            fs::create_directories(receiverInboxDir);

            if (fs::exists(senderLocalFile)) {
                
                fs::path destFile = receiverInboxDir / (sender + "_" + filename);
                fs::copy_file(senderLocalFile, destFile, fs::copy_options::overwrite_existing);
                cout << "File also copied locally to inbox as: " << destFile.string() << "\n";
            }
            else {
                cout << "File not found locally for sender: " << senderLocalFile.string() << "\n";
            }
        }
        catch (const fs::filesystem_error& e) {
            cerr << "Error copying file locally to inbox: " << e.what() << "\n";
        }

        return true;
    }
    else {
        cout << "Share failed: " << response << "\n";
        return false;
    }
}

bool SecureSharing::shareFile(const string& s, const string& r, const string& f) {
    setSender(s);
    setReceiver(r);
    setFilename(f);
    return shareFile();
}
