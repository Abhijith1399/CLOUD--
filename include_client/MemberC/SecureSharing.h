#ifndef SECURESHARING_H
#define SECURESHARING_H

#include <string>

class SecureSharing {
private:
    std::string sender;
    std::string receiver;
    std::string filename;

public:
    void setSender(std::string s);
    std::string getSender();

    void setReceiver(std::string r);
    std::string getReceiver();

    void setFilename(std::string f);
    std::string getFilename();

    bool shareFile(); // uses previously set values
    bool shareFile(const std::string& s, const std::string& r, const std::string& f); // full call
};

#endif
