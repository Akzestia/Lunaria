#!/bin/bash 


export LD_LIBRARY_PATH=./MsQuic/bin:$LD_LIBRARY_PATH

if ! g++ ./client/client.cpp -o./build/client \
	-L./MsQuic/bin -lmsquic \
	-lstdc++ \
	./client/QuicClient.cpp; then
	echo "Error: Compilation failed"
	exit 1
fi

echo "Compiled Successfully";
 
if ! LD_LIBRARY_PATH=./MsQuic/bin:$LD_LIBRARY_PATH ./build/client; then
	echo "Error: Client execution failed";
	exit 1
fi

echo "Client execution Succeded";
