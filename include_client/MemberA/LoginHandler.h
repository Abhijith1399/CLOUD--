#pragma once
#ifndef LOGIN_HANDLER_H
#define LOGIN_HANDLER_H

#include "Personal.h"

class LoginHandler : public Personal {
public:
	LoginHandler(std::string uname, std::string pwd);
	bool loginUser();
};

#endif