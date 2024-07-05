#include <cstring>
#include <cstdlib>
#include <cstdio>

class EncryptionManager
{
    EncryptionManager();
protected:
    bool encryptMessage(const char*, const char*, char**);
    bool decryptMessage(const char*, const char*, char**);
    bool generateKey(const char*, char**);
};