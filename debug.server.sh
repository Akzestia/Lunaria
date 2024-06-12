#!/bin/bash

if ! ./build/server; then
	echo "Error: Server debug failed"
	exit 1;
fi