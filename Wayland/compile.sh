#!/bin/bash 

g++ -g -o wayland main.cpp \
    -I./test.h \
    ./test.c \
    -lwayland-client -lwayland-server -lpng

if [ $? -ne 0 ]; then
    echo "Error: Compilation failed"
    exit 1
fi

if ! ./wayland; then
    echo "Error: Wayland execution failed"
    exit 1
fi

echo "Wayland execution Succeeded"
