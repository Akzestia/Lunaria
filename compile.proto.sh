#!/bin/bash 

export SRC_PROTO_DIR=./proto
export DST_PROTO_DIR=./proto/build

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/test.proto; then
    echo "Error: Proto failed \`test\`";
    exit 1
fi

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/user.proto; then
    echo "Error: Proto failed \`user\`";
    exit 1
fi

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/wrapper.proto; then
    echo "Error: Proto failed \`wrapper\`";
    exit 1
fi

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/message.proto; then
    echo "Error: Proto failed \`message\`";
    exit 1
fi

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/auth.proto; then
    echo "Error: Proto failed \`auth\`";
    exit 1
fi

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/server.proto; then
    echo "Error: Proto failed \`server\`";
    exit 1
fi

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/report.proto; then
    echo "Error: Proto failed \`report\`";
    exit 1
fi

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/vpn_graph.proto; then
    echo "Error: Proto failed \`vpn graph\`";
    exit 1
fi


if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/invite_link.proto; then
    echo "Error: Proto failed \`invite link\`";
    exit 1
fi

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/contact.proto; then
    echo "Error: Proto failed \`contact\`";
    exit 1
fi

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/encrypt_key.proto; then
    echo "Error: Proto failed \`encrypt key\`";
    exit 1
fi

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/sign_in.proto; then
    echo "Error: Proto failed \`sign in\`";
    exit 1
fi

if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/sign_up.proto; then
    echo "Error: Proto failed \`sign up\`";
    exit 1
fi


echo "Proto files were successfully compiled";