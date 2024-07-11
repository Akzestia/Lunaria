#!/bin/bash

# Create a symbolic link if it doesn't exist
if [ ! -f ../MsQuic/Linux_x64/bin/libmsquic.so ]; then
    ln -s ../MsQuic/Linux_x64/bin/libmsquic.so.2.3.5 ../MsQuic/Linux_x64/bin/libmsquic.so
fi

# Set the directory where libmsquic.so.2 resides
MSQUIC_LIB_DIR=$(pwd)/../MsQuic/Linux_x64/bin

# Check if LD_LIBRARY_PATH already contains MSQUIC_LIB_DIR
if [[ ":$LD_LIBRARY_PATH:" != *":$MSQUIC_LIB_DIR:"* ]]; then
    export LD_LIBRARY_PATH=$MSQUIC_LIB_DIR:$LD_LIBRARY_PATH
fi

# Export the library path
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)../MsQuic/Linux_x64/bin

# Compile the server
if ! g++ -g ./test.server.cpp -o./build/server \
    -I../proto \
    -L../MsQuic/Linux_x64/bin -lmsquic \
    -L/usr/lib -labsl_log_internal_check_op \
    -L/usr/lib -labsl_log_internal_message \
    -L/usr/lib -labsl_cord \
    -L/usr/lib -lpqxx \
    -L/usr/lib -lcrypto \
    -lstdc++ -lpthread -lprotobuf \
    ../server/QuicServer.cpp \
    ../route-manager/RouteManager.cpp \
    ../tokio-cpp/ThreadPool.cpp
    ../proto/build/test.pb.cc \
    ../proto/build/user.pb.cc \
    ../proto/build/message.pb.cc \
    ../proto/build/auth.pb.cc \
    ../proto/build/contact.pb.cc \
    ../proto/build/encrypt_key.pb.cc \
    ../proto/build/invite_link.pb.cc \
    ../proto/build/report.pb.cc \
    ../proto/build/server.pb.cc \
    ../proto/build/vpn_graph.pb.cc \
    ../proto/build/sign_in.pb.cc \
    ../proto/build/sign_up.pb.cc \
    ../proto/build/wrapper.pb.cc \
    ../Helpers/ConnectionManager/ConnectionManager.cpp \
    ../Helpers/Encryption/EncryptionManager.cpp \
    ../db/DbManager.cpp \
    ../Helpers/PeerHandler/PeerHandler.cpp \
    -Wl,-rpath,$MSQUIC_LIB_DIR; then
    echo "Error: Compilation failed"
    exit 1
fi

echo "Compiled Successfully"

# Run the server
if ! ./build/server; then
    echo "Error: Server execution failed"
    exit 1
fi

echo "Server execution succeeded"
