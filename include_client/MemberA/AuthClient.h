#ifndef AUTHCLIENT_H
#define AUTHCLIENT_H

#include <string>

class AuthClient
{
private:
	std::string username;
	std::string password;

public:

	AuthClient() = default;
	AuthClient(const std::string& uname, const std::string& pword);



	void setUsername(const std::string& uname);
	void setPassword(const std::string& pword);


	std::string getUsername() const;
	std::string getPassword() const;


	bool login(const std::string& uname, const std::string& pword);

	bool registerUser(const std::string& uname, const std::string& pword, const std::string& email,
		const std::string& address, const std::string& phone, const std::string& name);

	void logout();
};

#endif