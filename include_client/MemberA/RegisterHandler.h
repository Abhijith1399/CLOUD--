#ifndef REGISTER_HANDLER_H
#define REGISTER_HANDLER_H

#include "Personal.h"

class RegisterHandler : public Personal {
public:
	RegisterHandler(std::string uname, std::string pwd, std::string mail, std::string addr, std::string ph, std::string nm);
	void registerUser();
};

#endif