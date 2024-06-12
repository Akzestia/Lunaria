#!/bin/bash

modified_headers=$(git diff --name-only $GITHUB_SHA HEAD -- '*.h')
modified_cpp=$(git diff --name-only $GITHUB_SHA HEAD -- '*.cpp')
modified_proto=$(git diff --name-only $GITHUB_SHA HEAD -- '*.proto')

# Process each modified header file
for header in $modified_headers; do
    echo "Processing $header"
    #todo
done

# Process each modified cpp file
for cpp in $modified_headers; do
    echo "Processing $cpp"
    #todo
done

# Process each modified proto file
for proto in $modified_headers; do
    echo "Processing $proto"
    #todo
done