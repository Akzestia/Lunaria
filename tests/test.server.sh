#!/bin/bash

# Create a symbolic link if it doesn't exist
if [ ! -f ../MsQuic/bin/libmsquic.so ]; then
    ln -s ../MsQuic/bin/libmsquic.so.2.3.5 ../MsQuic/bin/libmsquic.so
fi

# Export the library path
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)../MsQuic/bin

# Compile the server
if ! g++ -g ./test.server.cpp -o./build/server \
    -I../proto \
    -L../MsQuic/bin -lmsquic \
    -L/usr/lib -labsl_log_internal_check_op \
    -L/usr/lib -labsl_log_internal_message \
    -L/usr/lib -labsl_cord \
    -L/usr/lib -lpqxx \
    -lstdc++ -lpthread -lprotobuf \
    ../server/QuicServer.cpp \
    ../proto/build/test.pb.cc \
    ../proto/build/user.pb.cc \
    ../proto/build/message.pb.cc \
    ../proto/build/wrapper.pb.cc \
    ../Helpers/ConnectionManager/ConnectionManager.cpp \
    ../db/DbManager.cpp \
    ../Helpers/PeerHandler/PeerHandler.cpp; then
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
