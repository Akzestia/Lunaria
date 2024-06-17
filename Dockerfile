# Use Arch Linux as the base image
FROM archlinux:latest

# Set the working directory inside the container
WORKDIR /app

# Update package repositories and install necessary packages
RUN pacman -Syu --noconfirm \
    && pacman -S --noconfirm gcc make boost libpqxx openssl protobuf numactl

# Copy the current directory contents into the container at /app
COPY . /app

# Set up symbolic link for libmsquic.so (if necessary)
RUN ln -sf /app/MsQuic/bin/libmsquic.so.2.3.5 /app/MsQuic/bin/libmsquic.so

# Export the library path and update linker cache
ENV LD_LIBRARY_PATH=/app/MsQuic/bin:/usr/lib

# Set environment variables for protobuf compilation
ENV SRC_PROTO_DIR=/app/proto
ENV DST_PROTO_DIR=/app/proto/build

# Compile protobuf files
RUN if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/test.proto; then \
        echo "Error: Proto failed \`test\`"; \
        exit 1; \
    fi \
    && if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/user.proto; then \
        echo "Error: Proto failed \`user\`"; \
        exit 1; \
    fi \
    && if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/wrapper.proto; then \
        echo "Error: Proto failed \`wrapper\`"; \
        exit 1; \
    fi \
    && if ! protoc -I=$SRC_PROTO_DIR --cpp_out=$DST_PROTO_DIR $SRC_PROTO_DIR/message.proto; then \
        echo "Error: Proto failed \`message\`"; \
        exit 1; \
    fi \
    && echo "Proto files were successfully compiled"

# Compile the server with explicit linking to OpenSSL, libnuma, and boost
RUN g++ -g /app/server/server.cpp -o /app/build/server \
    -I/app/proto/build \
    -I/usr/include/boost \
    -I/usr/include/postgresql \
    -L/app/MsQuic/bin -Wl,-rpath=/app/MsQuic/bin -lmsquic \
    -L/usr/lib -Wl,-rpath=/usr/lib -labsl_log_internal_check_op \
    -L/usr/lib -Wl,-rpath=/usr/lib -labsl_log_internal_message \
    -L/usr/lib -Wl,-rpath=/usr/lib -labsl_cord \
    -L/usr/lib -Wl,-rpath=/usr/lib -lpqxx \
    -L/usr/lib -Wl,-rpath=/usr/lib -lssl \
    -L/usr/lib -Wl,-rpath=/usr/lib -lcrypto \
    -lstdc++ -lpthread -lprotobuf \
    /app/server/QuicServer.cpp \
    /app/proto/build/test.pb.cc \
    /app/proto/build/user.pb.cc \
    /app/proto/build/wrapper.pb.cc \
    /app/proto/build/message.pb.cc \
    /app/Helpers/ConnectionManager.cpp \
    /app/db/DbManager.cpp \
    /app/Helpers/PeerHandler.cpp

# Expose the port the server will run on
EXPOSE 6121

# Run the server program
CMD ["/app/build/server"]
