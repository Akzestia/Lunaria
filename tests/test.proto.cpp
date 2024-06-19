#include "../proto/build/user.pb.h"

bool operator<(const User& lhs, const User& rhs) {
    return lhs.name() < rhs.name();
}

bool operator>(const User& lhs, const User& rhs) {
    return lhs.name() > rhs.name();
}


bool operator==(const User& lhs, const User& rhs) {
    return lhs.name() == rhs.name() &&
           lhs.email() == rhs.email() &&
           lhs.avatar() == rhs.avatar();
}


int main(){

    User u1, u2;
    u1.set_name("X");
    u2.set_name("Y");

    std::cout << (u1 > u2) << "\n";
    std::cout << (u1 < u2) << "\n";
    std::cout << (u2 > u1) << "\n";
    std::cout << (u2 < u1) << "\n";
    
    return 0;
}