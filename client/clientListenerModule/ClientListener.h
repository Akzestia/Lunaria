#ifndef CLIENT_LISTENER_H
#define CLIENT_LISTENER_H
#include "../../MsQuic/Linux_x64/include/msquic.h"

class ClientListener{
    public:

    protected:
        virtual ~ClientListener();
    private:
        ClientListener() = default;
        friend class QuicClient;
};

#endif