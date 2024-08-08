#!/bin/bash

# Set the Cassandra connection details
CASSANDRA_HOST="127.0.0.1"
CASSANDRA_PORT="9042"
CASSANDRA_USER="casandra"
CASSANDRA_PASSWORD="casandra"

# Connect to Cassandra and get a list of keyspaces
keyspaces=$(cqlsh -u $CASSANDRA_USER -p $CASSANDRA_PASSWORD -e "DESCRIBE KEYSPACES;" | grep -v "system\|system_auth\|system_traces\|system_distributed\|system_schema")

# Loop through the keyspaces and drop them
for keyspace in $keyspaces; do
    echo "Dropping keyspace: $keyspace"
    cqlsh -u $CASSANDRA_USER -p $CASSANDRA_PASSWORD -e "DROP KEYSPACE $keyspace;"
done

echo "All keyspaces and tables have been deleted."
