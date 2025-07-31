#include "MemberC/base/TrashBase.h"
#include <iostream>


using namespace std;

void TrashBase::logTrashAction(const string& user, const string& action) {

    //it records whenever user perform an action on files

    cout << "Log: " << user << " performed " << action << " on trash" << endl;

}
