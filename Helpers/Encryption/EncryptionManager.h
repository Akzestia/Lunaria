#include <cstring>
#include <cstdlib>

class EncryptionManager
{
protected:
    bool encryptMessage(const char*, const char*, char**);
    bool decryptMessage(const char*, const char*, char**);
};