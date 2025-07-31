#include "MemberA/Personal.h"
#include <iostream>

Personal::Personal() {}

Personal::Personal(std::string uname, std::string pwd, std::string mail, std::string addr, std::string ph, std::string nm)
	: username(uname), password(pwd), email(mail), address(addr), phone(ph), name(nm) {}

void Personal::displayProfile() const {
	std::cout << "\nName: " << name << "\nEmail: " << email << "\nPhone: " << phone << "\nAddress: " << address << "\n";
}

std::string Personal::getUsername() const { return username; }
std::string Personal::getPassword() const { return password; }
std::string Personal::getEmail() const { return email; }
std::string Personal::getPhone() const { return phone; }