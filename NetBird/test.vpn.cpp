#include "VpnManager.h"

int main() {
    std::set<std::string> interfaces = VpnManager::getInterfaces();

    for (auto it = interfaces.begin(); it != interfaces.end(); ++it) {
        std::cout << "Interface: " << *it << std::endl;
    }

    std::cout << *interfaces.begin() << std::endl;
    return 0;
}