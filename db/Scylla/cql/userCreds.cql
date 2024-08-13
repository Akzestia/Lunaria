CREATE KEYSPACE IF NOT EXISTS lunnaria_service
WITH replication = {
    'class': 'SimpleStrategy',
    'replication_factor': 1
};

USE lunnaria_service;

CREATE TABLE IF NOT EXISTS UserCredentials (
    id UUID,
    user_name TEXT PRIMARY KEY,
    user_password TEXT,
    user_email TEXT,
);


CREATE TABLE IF NOT EXISTS ExistingEmails (
    user_email TEXT,
    PRIMARY KEY((user_email))
);
