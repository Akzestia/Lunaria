cmake_minimum_required(VERSION 3.10)
project(VpnManagerproject)

add_executable(vpn
    ./test.vpn.cpp
    ${CMAKE_SOURCE_DIR}/VpnManager.cpp
)