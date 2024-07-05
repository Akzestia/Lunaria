#include <cstdio>
#include <cstdlib>
#include <cstring>

class EncryptionManager {
    EncryptionManager() = default;
    friend class QuicServer;

  protected:
    bool encryptMessage(const char *, const char *, char **);
    bool decryptMessage(const char *, const char *, char **);
    bool generateKey(const char *, char **);
};