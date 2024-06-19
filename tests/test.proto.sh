


if ! g++ -g ./test.proto.cpp -o ./build/proto \
    ../proto/build/user.pb.cc \
    -L/usr/lib -labsl_log_internal_check_op \
    -L/usr/lib -labsl_log_internal_message \
    -L/usr/lib -labsl_cord \
    -L/usr/lib -lpqxx \
    -lstdc++ -lpthread -lprotobuf; then
    echo "Error: Compilation failed"
    exit 1
fi

echo "Compiled Successfully"

# Run the proto
if ! ./build/proto; then
    echo "Error: Proto execution failed"
    exit 1
fi

echo "Proto execution succeeded"
