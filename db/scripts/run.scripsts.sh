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
    "../Postgress/sql/users.sql"
    "../Postgress/sql/servers.sql"
    "../Postgress/sql/contacts.sql"
    "../Postgress/sql/messages.sql"
    "../Postgress/sql/blocked_users.sql"
    "../Postgress/sql/reports.sql"
    "../Postgress/sql/calls.sql"
    "../Postgress/sql/encrypt_keys.sql"
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
