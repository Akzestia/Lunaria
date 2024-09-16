cmake_minimum_required(VERSION 3.10)
project(QuicServerProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)

option(USE_SCYLLA_DB "Use ScyllaDB instead of PostgreSQL" OFF)

# Set the correct directory for the MsQuic library
set(MSQUIC_LIB_DIR ${CMAKE_SOURCE_DIR}/../MsQuic/Linux_x64/bin)
set(PROTO_BUILD_DIR ${CMAKE_SOURCE_DIR}/../proto/build)
set(HELPERS_DIR ${CMAKE_SOURCE_DIR}/../Helpers)
set(DB_DIR ${CMAKE_SOURCE_DIR}/../db)
set(PROTO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../proto)
set(ROUTE_MANAGER_DIR ${CMAKE_SOURCE_DIR}/../route-manager)
set(TOKIO_CPP_DIR ${CMAKE_SOURCE_DIR}/../tokio-cpp)

# Add symbolic links if they don't exist
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${MSQUIC_LIB_DIR}/libmsquic.so.2.3.5 ${MSQUIC_LIB_DIR}/libmsquic.so)

# Include directories
include_directories(${PROTO_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS} ${PROTO_BUILD_DIR} /usr/include/openssl-1.1)

# Add executable
add_executable(server
    ./test.server.cpp
    ${CMAKE_SOURCE_DIR}/../server/QuicServer.cpp
    ${ROUTE_MANAGER_DIR}/RouteManager.cpp
    ${TOKIO_CPP_DIR}/ThreadPool.cpp
    ${PROTO_BUILD_DIR}/user.pb.cc
    ${PROTO_BUILD_DIR}/message.pb.cc
    ${PROTO_BUILD_DIR}/auth.pb.cc
    ${PROTO_BUILD_DIR}/contact.pb.cc
    ${PROTO_BUILD_DIR}/encrypt_key.pb.cc
    ${PROTO_BUILD_DIR}/invite_link.pb.cc
    ${PROTO_BUILD_DIR}/report.pb.cc
    ${PROTO_BUILD_DIR}/server.pb.cc
    ${PROTO_BUILD_DIR}/vpn_graph.pb.cc
    ${PROTO_BUILD_DIR}/sign_in.pb.cc
    ${PROTO_BUILD_DIR}/sign_up.pb.cc
    ${PROTO_BUILD_DIR}/wrapper.pb.cc
    ${PROTO_BUILD_DIR}/authResponse.pb.cc
    ${PROTO_BUILD_DIR}/rpc_response.pb.cc
    ${PROTO_BUILD_DIR}/rpc_request.pb.cc
    ${PROTO_BUILD_DIR}/rpc_body.pb.cc
    ${PROTO_BUILD_DIR}/error.pb.cc
    ${PROTO_BUILD_DIR}/fetch_response.pb.cc
    ${HELPERS_DIR}/ConnectionManager/ConnectionManager.cpp
    ${HELPERS_DIR}/AuthManager/AuthManager.cpp
    ${HELPERS_DIR}/Encryption/EncryptionManager.cpp
    ${HELPERS_DIR}/ArenaMemoryResource/ArenaMemoryResource.hpp
    ${DB_DIR}/Scylla/ScyllaManager.cpp
    ${HELPERS_DIR}/PeerHandler/PeerHandler.cpp
)

# Link libraries
target_link_libraries(server
    ${MSQUIC_LIB_DIR}/libmsquic.so
    protobuf::libprotobuf
    absl::log_internal_check_op
    absl::log_internal_message
    absl::cord
    crypto
    ssl
    numa
    stdc++
    pthread
    cassandra
)

set(SOURCE_INI ${CMAKE_SOURCE_DIR}/../db/config.ini)
set(DESTINATION_INI ${CMAKE_BINARY_DIR}/config.ini)

add_custom_command(
    TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_INI} ${DESTINATION_INI}
    COMMENT "Copying config.ini to ${DESTINATION_INI}"
)

if(USE_SCYLLA_DB)
    target_compile_definitions(server PRIVATE USE_SCYLLA_DB)
endif()

# Set RPATH
set_target_properties(server PROPERTIES
    BUILD_RPATH ${MSQUIC_LIB_DIR}
)

# Print the library search path for debugging
message(STATUS "Library search path: ${CMAKE_LIBRARY_PATH}")
message(STATUS "MSQUIC library path: ${MSQUIC_LIB_DIR}")
message(STATUS "PROTOBUF library path: ${PROTOBUF_LIB_DIR}")
