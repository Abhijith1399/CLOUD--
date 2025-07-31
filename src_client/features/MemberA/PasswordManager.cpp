#include "MemberA/PasswordManager.h"
using namespace std;

string PasswordManager::hashPassword(const string& password)
{
    string hashed = password;
    for (char& c : hashed) c += 1;
    return hashed;
}
