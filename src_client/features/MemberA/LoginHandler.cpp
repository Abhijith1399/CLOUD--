#include"MemberA/LoginHandler.h"
#include "MemberA/PasswordManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include<ctime>
#include<cstring>

LoginHandler::LoginHandler(std::string uname, std::string pwd)
	: Personal("", uname, pwd, "", "", "") {}

bool LoginHandler::loginUser() {
	bool loginsucess = true;
	std::ifstream in("data/users.txt");
	std::string line;
	std::string hashedInputPwd = PasswordManager::hashPassword(password);
	while (getline(in, line)) {
		std::istringstream iss(line);
		std::string nm, u, p, mail, ph, addr;
		iss >> nm >> u >> p >> mail >> ph >> addr;
		if (u == username && p == hashedInputPwd) {
			std::cout << "Login Successful! Welcome, " << u << "\n";
			return true;
			break;
		}
	}
	std::cout << "Invalid Credentials.\n";
	return false;

	std::ofstream logFile("data/logs/loginlog.txt", std::ios::app);
	if (logFile.is_open()) {
		std::time_t now = std::time(nullptr);
		char dt[26];                            
		ctime_s(dt, sizeof(dt), &now);           
		dt[strlen(dt) - 1] = '\0';
		logFile << dt << " - Username: " << username
			<< " - Status: " << (loginsucess ? "Success" : "Failure") << "\n";
		logFile.close();
	}
	else {
		std::cerr << "Unable to open log file." << std::endl;
	}

	if (!loginsucess) {
		std::cout << "Invalid Credentials.\n";
	}

	return loginsucess;
};