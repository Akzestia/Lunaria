#!/bin/bash

export LD_LIBRARY_PATH=./MsQuic/bin:$LD_LIBRARY_PATH

if ! g++ ./server/server.cpp -o./build/server \
	-L./MsQuic/bin -lmsquic \
	-lstdc++ \
	./server/QuicServer.cpp; then 
	echo "Error: Compilation failed"
	exit 1
fi

echo "Compiled Successfully";

if ! LD_LIBRARY_PATH=./MsQuic/bin:$LDLIBRARY_PATH ./build/server; then
	echo "Error: Server execution failed"
	exit 1;
fi

echo "Server execution succeeded";

