#!/bin/bash

usage() {
    echo "Usage: $0 -u <username> -d <database>"
    exit 1
}

USERNAME=""
DATABASE=""

while getopts "u:d:" opt; do
    case "$opt" in
        u) USERNAME="$OPTARG" ;;
        d) DATABASE="$OPTARG" ;;
        *) usage ;;
    esac
done

if [ -z "$USERNAME" ] || [ -z "$DATABASE" ]; then
    usage
fi

SCRIPTS=(
    "./sql/users.sql"
    "./sql/servers.sql"
    "./sql/contacts.sql"
    "./sql/messages.sql"
    "./sql/blocked_users.sql"
    "./sql/reports.sql"
    "./sql/calls.sql"
    "./sql/encrypt_keys.sql"
)

execute_script() {
    local script=$1
    echo "Executing $script..."
    psql -U "$USERNAME" -d "$DATABASE" -f "$script"
    if [ $? -ne 0 ]; then
        echo "Error executing script: $script"
        exit 1
    fi
}

for script in "${SCRIPTS[@]}"; do
    execute_script "$script"
done

echo "All scripts executed successfully."
