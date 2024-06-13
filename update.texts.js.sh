#!/bin/bash

# Get the list of modified header, cpp, and proto files from environment variable
modified_headers=$(echo $FILES | tr ' ' '\n' | grep '\.h$')
modified_cpp=$(echo $FILES | tr ' ' '\n' | grep '\.cpp$')
modified_proto=$(echo $FILES | tr ' ' '\n' | grep '\.proto$')

echo "Modified headers:"
echo "$modified_headers"

echo "Modified cpp files:"
echo "$modified_cpp"

echo "Modified proto files:"
echo "$modified_proto"

# Check if the directory exists
if [ ! -d github-pages/js ]; then
    echo "Directory github-pages/js does not exist."
fi

# Check if texts.js exists, if not create one
if [ ! -f github-pages/js/test.js ]; then
    echo "Creating test.js"
    echo -n > ./github-pages/js/test.js
    touch github-pages/js/test.js
    echo "Creating test.js ended"
fi

process_file() {
    local file=$1
    local inside_class=false
    local class_content=""
    local class_name=""
    
    echo "Processing file: $file"
    
    while IFS= read -r line || [ -n "$line" ]; do
        if [[ $line == class* && $inside_class == false ]]; then
            inside_class=true
            class_content="$line"
            class_name=$(echo "$line" | grep -oP 'class \K\w+')
        elif [[ $inside_class == true ]]; then
            class_content="$class_content"$'\n'"$line"
        fi
    done < "$file"

    if [ "$inside_class" = true ]; then
        echo "Adding const $class_name to test.js"
        echo "const $class_name = \`$class_content\`;" >> github-pages/js/test.js
    fi
}

# Process each modified header file
for header in $modified_headers; do
    process_file $header
done

# Process each modified cpp file
for cpp in $modified_cpp; do
    process_file $cpp
done

# Process each modified proto file
for proto in $modified_proto; do
    process_file $proto
done

echo "Finished processing files"
