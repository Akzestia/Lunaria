#!/bin/bash

if ! g++ -g ./server/server.cpp -o./build/server \
	-I./proto \
	-L./MsQuic/bin -lmsquic \
	-L/usr/lib -labsl_log_internal_check_op \
	-L/usr/lib -labsl_log_internal_message \
	-L/usr/lib -labsl_cord \
	-lstdc++ -lpthread -lprotobuf \
	./server/QuicServer.cpp \
	./proto/build/test.pb.cc \
	./proto/build/user.pb.cc \
	./proto/build/wrapper.pb.cc \
	./Helpers/ConnectionManager.cpp \
	./Helpers/PeerHandler.cpp; then 
	echo "Error: Compilation failed"
	exit 1
fi

echo "Compiled Successfully";

if ! ./build/server; then
	echo "Error: Server execution failed"
	exit 1;
fi

echo "Server execution succeeded";

