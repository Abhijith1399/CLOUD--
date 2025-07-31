#include"MemberA/RegisterHandler.h"
#include "MemberA/UserValidator.h"
#include "MemberA/PasswordManager.h"
#include <fstream>
#include <iostream>

RegisterHandler::RegisterHandler(std::string uname, std::string pword, std::string mail, std::string addr, std::string ph, std::string nm)
	: Personal(uname, pword, mail, addr, ph, nm) {}

void RegisterHandler::registerUser() {
	if (!UserValidator::isValidEmail(email) || !UserValidator::isValidPhone(phone)) {
		std::cout << "Invalid email or phone number.\n";
		return;
	}
	std::ofstream out("data/users.txt", std::ios::app);
	if (!out) {
		std::cerr << "Could not open user file.\n";
		return;
	}
	std::string hashedPwd = PasswordManager::hashPassword(password);
	out << username << " " << hashedPwd << " " << email << " " << phone << " " << address << " " << name << "\n";
	std::cout << "Registered Successfully!\n";
}

