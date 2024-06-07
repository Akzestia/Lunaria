#!/bin/bash 

if ! g++ -g -o ./build/client \
    -I./proto \
    -I/usr/include/gtk-4.0 \
    -L/usr/lib/gtk-4.0/4.0.0/immodules -lim-ibus \
    -L/usr/lib/gtk-4.0/4.0.0/media -lmedia-gstreamer \
    -L/usr/lib/gtk-4.0/4.0.0/printbackends -lprintbackend-cups \
    -L/usr/lib/gtk-4.0/4.0.0/printbackends -lprintbackend-file \
    -L/usr/lib -lX11 \
    -L./MsQuic/bin -lmsquic \
	-L/usr/lib -labsl_log_internal_check_op \
	-L/usr/lib -labsl_log_internal_message \
	-L/usr/lib -labsl_cord \
    ./client/client.cpp \
    ./client/QuicClient.cpp \
    ./proto/build/test.pb.cc \
	./proto/build/user.pb.cc \
	./proto/build/wrapper.pb.cc \
    ./Helpers/ConnectionManager.cpp \
    ./Helpers/ScreenCapture.cpp \
    -lstdc++ -lpthread -lprotobuf; then
    echo "Error: Compilation failed"
    exit 1
fi

echo "Compiled Successfully"

export LD_LIBRARY_PATH=/usr/lib/gtk-4.0/4.0.0/immodules:/usr/lib/gtk-4.0/4.0.0/media:/usr/lib/gtk-4.0/4.0.0/printbackends:$LD_LIBRARY_PATH

# Run the compiled client
if ! ./build/client; then
    echo "Error: Client execution failed"
    exit 1
fi

echo "Client execution Succeeded"
