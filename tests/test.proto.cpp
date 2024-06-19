#include "../proto/build/user.pb.h"
#include <set>
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
    std::set<User>* s = new std::set<User>();

/*     User u1, u2;
    u1.set_name("X");
    u2.set_name("Y");

    std::cout << (u1 > u2) << "\n";
    std::cout << (u1 < u2) << "\n";
    std::cout << (u2 > u1) << "\n";
    std::cout << (u2 < u1) << "\n"; */

    
    for (char c = 'Z'; c >= 'W'; --c) {
        User u;
        u.set_name(std::string(1, c));
        s->insert(u);
    }

    for (char c = 'A'; c <= 'D'; ++c) {
        User u;
        u.set_name(std::string(1, c));
        s->insert(u);
    }

    for (const auto& user : *s) {
        std::cout << "User name: " << user.name() << "\n";
    }
    delete s;
    return 0;
}