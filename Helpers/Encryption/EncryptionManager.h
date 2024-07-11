#ifndef ENCRYPTION_MANAGER_H
#define ENCRYPTION_MANAGER_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <openssl/sha.h>
#include <string>
#include "../../client/clientRequestManager/RequestManager.h"

class EncryptionManager {
    EncryptionManager() = default;
    friend class QuicServer;
    friend class RequestManager;
    
  protected:
    static bool encryptMessage(const char *, const char *, char **);
    static bool decryptMessage(const char *, const char *, char **);
    static bool generateKey(const char *, char **);
    static bool ToSHA256(const std::string &input, std::string &output);
};
#pragma GCC diagnostic pop

#endif