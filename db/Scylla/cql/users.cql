CREATE KEYSPACE IF NOT EXISTS lunnaria_service
WITH replication = {
    'class': 'SimpleStrategy',
    'replication_factor': 1
};

USE lunnaria_service;

CREATE TABLE IF NOT EXISTS Users (
    id UUID,
    display_name TEXT,
    user_name TEXT,
    user_email TEXT,
    user_avatar BLOB,
    online_status INT,
    joined_at TIMESTAMP,
    last_activity TIMESTAMP,

    PRIMARY KEY((user_name))
);
