CREATE KEYSPACE IF NOT EXISTS lunnaria_service
WITH replication = {
    'class': 'SimpleStrategy',
    'replication_factor': 1
};

USE lunnaria_service;

CREATE TABLE IF NOT EXISTS Users (
    id UUID PRIMARY KEY,
    display_name TEXT,
    user_name TEXT,
    user_email TEXT,
    user_avatar BLOB,
    user_password TEXT,
    online_status INT,
    joined_at TIMESTAMP,
    last_activity TIMESTAMP
);


CREATE INDEX IF NOT EXISTS idx_users_user_name ON Users(user_name);
CREATE INDEX IF NOT EXISTS idx_users_user_email ON Users(user_email);
CREATE INDEX IF NOT EXISTS idx_users_online_status ON Users(online_status);
