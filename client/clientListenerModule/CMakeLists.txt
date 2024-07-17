cmake_minimum_required(VERSION 3.10)
project(QuicClientProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Protobuf REQUIRED)

# Set the correct directory for the MsQuic library
set(MSQUIC_LIB_DIR ${CMAKE_SOURCE_DIR}/../../MsQuic/Linux_x64/bin)
set(PROTOBUF_LIB_DIR ${CMAKE_SOURCE_DIR}/../../protobuf)
set(PROTO_BUILD_DIR ${CMAKE_SOURCE_DIR}/../../proto/build)
set(HELPERS_DIR ${CMAKE_SOURCE_DIR}/../../Helpers)
set(DB_DIR ${CMAKE_SOURCE_DIR}/../../db)
set(PROTO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../../proto)
set(ABSL_LIB_DIR ${CMAKE_SOURCE_DIR}/../../absl-libs)
set(ROUTE_MANAGER_DIR ${CMAKE_SOURCE_DIR}/../../route-manager)
set(TOKIO_CPP_DIR ${CMAKE_SOURCE_DIR}/../../tokio-cpp)

# Add symbolic links if they don't exist
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${MSQUIC_LIB_DIR}/libmsquic.so.2.3.5 ${MSQUIC_LIB_DIR}/libmsquic.so)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROTOBUF_LIB_DIR}/libprotobuf.so ${PROTOBUF_LIB_DIR}/libprotobuf.so.27)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${ABSL_LIB_DIR}/libabsl_log_internal_check_op.so ${ABSL_LIB_DIR}/libabsl_log_internal_check_op.so.2401.0.0)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${ABSL_LIB_DIR}/libabsl_log_internal_message.so ${ABSL_LIB_DIR}/libabsl_log_internal_message.so.2401.0.0)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${ABSL_LIB_DIR}/libabsl_cord.so ${ABSL_LIB_DIR}/libabsl_cord.so.2401.0.0)

# Include directories
include_directories(${PROTO_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS} ${PROTO_BUILD_DIR} /usr/include/openssl-1.1)

# Add executable
add_executable(client
    ./test.cpp
    ${CMAKE_SOURCE_DIR}/../QuicClient.cpp
    ${CMAKE_SOURCE_DIR}/ClientListener.cpp
    ${ROUTE_MANAGER_DIR}/RouteManager.cpp
    ${TOKIO_CPP_DIR}/ThreadPool.cpp
    ${PROTO_BUILD_DIR}/test.pb.cc
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
    ${HELPERS_DIR}/ConnectionManager/ConnectionManager.cpp
    ${HELPERS_DIR}/Encryption/EncryptionManager.cpp
    ${DB_DIR}/DbManager.cpp
    ${HELPERS_DIR}/PeerHandler/PeerHandler.cpp
)

# Link libraries
target_link_libraries(client
    ${MSQUIC_LIB_DIR}/libmsquic.so
    ${PROTOBUF_LIBRARIES}
    ${ABSL_LIB_DIR}/libabsl_log_internal_check_op.so
    ${ABSL_LIB_DIR}/libabsl_log_internal_message.so
    ${ABSL_LIB_DIR}/libabsl_cord.so
    pqxx
    crypto
    ssl
    numa
    stdc++
    pthread
)

set(SOURCE_INI ${CMAKE_SOURCE_DIR}/../../db/config.ini)
set(DESTINATION_INI ${CMAKE_BINARY_DIR}/config.ini)
set(SOURCE_CKEY ${CMAKE_SOURCE_DIR}/../../certs/server.key)
set(DESTINATION_CKEY ${CMAKE_BINARY_DIR}/certs/server.key)
set(SOURCE_CERT ${CMAKE_SOURCE_DIR}/../../certs/server.cert)
set(DESTINATION_CERT ${CMAKE_BINARY_DIR}/certs/server.cert)

add_custom_command(
    TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_INI} ${DESTINATION_INI}
    COMMENT "Copying config.ini to ${DESTINATION_INI}"
)

add_custom_command(
    TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_CKEY} ${DESTINATION_CKEY}
    COMMENT "Copying server.key to ${DESTINATION_CCERT}"
)

add_custom_command(
    TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_CERT} ${DESTINATION_CERT}
    COMMENT "Copying server.cert to ${DESTINATION_CERT}"
)

# Set RPATH
set_target_properties(client PROPERTIES
    BUILD_RPATH ${MSQUIC_LIB_DIR}
)

# Print the library search path for debugging
message(STATUS "Library search path: ${CMAKE_LIBRARY_PATH}")
message(STATUS "MSQUIC library path: ${MSQUIC_LIB_DIR}")
message(STATUS "PROTOBUF library path: ${PROTOBUF_LIB_DIR}")
