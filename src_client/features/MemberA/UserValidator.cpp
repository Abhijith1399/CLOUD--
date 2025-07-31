#include "MemberA/UserValidator.h"
#include <regex>

using namespace std;

bool UserValidator::isValidUsername(const string& username) {
    return username.length() >= 3;
}

bool UserValidator::isValidPassword(const string& password) {
    return password.length() >= 4;
}

bool UserValidator::isValidEmail(const string& email) {
    regex pattern(R"((\w+)(\.{0,1})(\w*)@(\w+)\.(\w+))");
    return regex_match(email, pattern);
}

bool UserValidator::isValidPhone(const string& phone) {
    regex pattern(R"(\d{10})");  // 10-digit numeric check
    return regex_match(phone, pattern);
}