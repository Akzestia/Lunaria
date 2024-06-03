#!/bin/bash 

export SRC_PROTO_DIR=./proto
export DST_PROTO_DIR=./proto

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/test.proto; then
    echo "Error: Proto failed"
    exit 1
fi

echo "Proto files were successfully compiled";