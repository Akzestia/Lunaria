#!/bin/bash

if ! g++ -g ./server/server.cpp -o./build/server \
	-L./MsQuic/bin -lmsquic \
	-lstdc++ \
	./server/QuicServer.cpp \
	./Entities/Message.cpp \
	./Entities/User.cpp \
	./Entities/Contact.cpp; then 
	echo "Error: Compilation failed"
	exit 1
fi

echo "Compiled Successfully";

if ! ./build/server; then
	echo "Error: Server execution failed"
	exit 1;
fi

echo "Server execution succeeded";

