#!/bin/bash 

export SRC_PROTO_DIR=./proto
export DST_PROTO_DIR=./proto

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/test.proto; then
    echo "Error: Proto failed"
    exit 1
fi

if ! g++ -g -o ./build/client \
    -I./proto \
    -L./MsQuic/bin -lmsquic \
	-L/usr/lib -labsl_log_internal_check_op \
	-L/usr/lib -labsl_log_internal_message \
	-L/usr/lib -labsl_cord \
    ./client/client.cpp \
    ./client/QuicClient.cpp \
    ./Entities/Message.cpp \
    ./Entities/User.cpp \
    ./Entities/Contact.cpp \
    ./proto/test.pb.cc \
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

echo "Client execution Succeeded"
