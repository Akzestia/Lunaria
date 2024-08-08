#!/bin/bash

# Usage function to display help
usage() {
    echo "Usage: $0 -u <username> -p <password> -s <schema_directory>"
    exit 1
}

# Default values for variables
USERNAME=""
PASSWORD=""
SCHEMA_DIR=""

# Parse command-line options
while getopts "u:p:s:" opt; do
    case "$opt" in
        u) USERNAME="$OPTARG" ;;
        p) PASSWORD="$OPTARG" ;;
        s) SCHEMA_DIR="$OPTARG" ;;
        *) usage ;;
    esac
done

# Check if required variables are set
if [ -z "$USERNAME" ] || [ -z "$PASSWORD" ] || [ -z "$SCHEMA_DIR" ]; then
    usage
fi

# Function to execute a .cql script
execute_script() {
    local script=$1
    echo "Executing $script..."
    /home/azure/scylladb/tools/cqlsh/bin/cqlsh.py -u "$USERNAME" -p "$PASSWORD" -f "$script"
    if [ $? -ne 0 ]; then
        echo "Error executing script: $script"
        exit 1
    fi
}

# Check if schema directory exists
if [ ! -d "$SCHEMA_DIR" ]; then
    echo "Schema directory does not exist: $SCHEMA_DIR"
    exit 1
fi

# Loop through .cql files and execute them
for script in "$SCHEMA_DIR"/*.cql; do
    if [ -f "$script" ]; then
        execute_script "$script"
    else
        echo "No .cql files found in directory: $SCHEMA_DIR"
        exit 1
    fi
done

echo "All scripts executed successfully."
