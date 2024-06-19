#!/bin/bash

# Ensure libmsquic.so symbolic link is created if it doesn't exist
if [ ! -f ../MsQuic/bin/libmsquic.so ]; then
    ln -s ../MsQuic/bin/libmsquic.so.2.3.5 ../MsQuic/bin/libmsquic.so
fi

# Set the directory where libmsquic.so.2 resides
MSQUIC_LIB_DIR=$(pwd)../MsQuic/bin

# Check if LD_LIBRARY_PATH already contains MSQUIC_LIB_DIR
if [[ ":$LD_LIBRARY_PATH:" != *":$MSQUIC_LIB_DIR:"* ]]; then
    export LD_LIBRARY_PATH=$MSQUIC_LIB_DIR:$LD_LIBRARY_PATH
fi

# Set the directory where libim-ibus.so resides
export LD_LIBRARY_PATH=/usr/lib/gtk-4.0/4.0.0/immodules:/usr/lib/gtk-4.0/4.0.0/media:/usr/lib/gtk-4.0/4.0.0/printbackends:$LD_LIBRARY_PATH

# Compile the client
if ! g++ -g ./test.client.cpp -o ./build/client \
    -I../proto \
    -I/usr/include/gtk-4.0 \
    -L/usr/lib/gtk-4.0/4.0.0/immodules -lim-ibus \
    -L/usr/lib/gtk-4.0/4.0.0/media -lmedia-gstreamer \
    -L/usr/lib/gtk-4.0/4.0.0/printbackends -lprintbackend-cups \
    -L/usr/lib/gtk-4.0/4.0.0/printbackends -lprintbackend-file \
    -L/usr/lib -lX11 \
    -L/usr/lib -lpqxx \
    -L../MsQuic/bin -lmsquic \
    -L/usr/lib -labsl_log_internal_check_op \
    -L/usr/lib -labsl_log_internal_message \
    -L/usr/lib -labsl_cord \
    ../client/QuicClient.cpp \
    ../proto/build/test.pb.cc \
    ../proto/build/user.pb.cc \
    ../proto/build/message.pb.cc \
    ../proto/build/wrapper.pb.cc \
    ../Helpers/ConnectionManager.cpp \
    ../Helpers/ScreenCapture.cpp \
    ../db/DbManager.cpp \
    -lstdc++ -lpthread -lprotobuf; then
    echo "Error: Compilation failed"
    exit 1
fi

echo "Compiled Successfully"

# Run the compiled client
if ! ./build/client; then
    echo "Error: Client execution failed"
    exit 1
fi

echo "Client execution succeeded"
