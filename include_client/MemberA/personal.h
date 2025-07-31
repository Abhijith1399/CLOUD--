#ifndef PERSONAL_H
#define PERSONAL_H

#include <string>

class Personal {
protected:
	std::string name;
	std::string username;
	std::string password;
	std::string email;
	std::string address;
	std::string phone;
public:
	Personal();
	Personal(std::string uname, std::string pwd, std::string mail, std::string addr, std::string ph, std::string nm);
	void displayProfile() const;
	std::string getUsername() const;
	std::string getPassword() const;
	std::string getEmail() const;
	std::string getPhone() const;
};

#endif