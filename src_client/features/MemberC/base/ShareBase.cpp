#include <iostream>
#include "MemberC/base/ShareBase.h"



using namespace std;


bool ShareBase::validateReceiver(const string& receiver) {

    return !receiver.empty();//it checks receiver name is not empty

}
