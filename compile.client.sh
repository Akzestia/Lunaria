#!/bin/bash 

export LD_LIBRARY_PATH=./MsQuic/bin:$LD_LIBRARY_PATH
export SRC_PROTO_DIR=./proto
export DST_PROTO_DIR=./proto

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/test.proto; then
	echo "Error: Proto failed"
	exit 1
fi

ABSL_DYNAMIC_ANNOTATIONS_CFLAGS=$(pkg-config --cflags absl_dynamic_annotations)
ABSL_DYNAMIC_ANNOTATIONS_LDFLAGS=$(pkg-config --libs absl_dynamic_annotations)

if ! g++ -g -o ./build/client \
    ./client/client.cpp \
    ./client/QuicClient.cpp \
    ./Entities/Message.cpp \
    ./Entities/User.cpp \
    ./Entities/Contact.cpp \
    ./proto/test.pb.cc \
    -L./MsQuic/bin -lmsquic \
    -I./proto \
    -lstdc++ -lpthread -lprotobuf \
    -labsl_log_internal_check_op \
    -labsl_base -labsl_spinlock_wait \
    -labsl_raw_logging_internal \
    -labsl_throw_delegate \
    -labsl_demangle_internal \
    -labsl_log_severity \
    -labsl_dynamic_annotations \
    -labsl_symbolize -labsl_cord \
    -ldl -pthread \
    -L/usr/lib -labsl_dynamic_annotations
	$ABSL_DYNAMIC_ANNOTATIONS_LDFLAGS \
	$ABSL_DYNAMIC_ANNOTATIONS_CFLAGS; then
    echo "Error: Compilation failed"
    exit 1
fi

echo "Compiled Successfully"

# Run the compiled client
if ! LD_LIBRARY_PATH=./MsQuic/bin:$LD_LIBRARY_PATH ./build/client; then
    echo "Error: Client execution failed"
    exit 1
fi

echo "Client execution Succeeded"
This script includes additional absl libraries that might be required for linking. Make sure these libraries are installed on your system. If the issue persists, you might need to adjust the library names or paths according to your system configuration.








echo "Compiled Successfully";
 
if ! LD_LIBRARY_PATH=./MsQuic/bin:$LD_LIBRARY_PATH ./build/client; then
	echo "Error: Client execution failed";
	exit 1
fi

echo "Client execution Succeded";
