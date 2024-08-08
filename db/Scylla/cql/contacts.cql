CREATE KEYSPACE IF NOT EXISTS lunnaria_service
WITH replication = {
    'class': 'SimpleStrategy',
    'replication_factor': 1
};

USE lunnaria_service;

CREATE TABLE IF NOT EXISTS Contacts (
    id UUID,
    a_user_id UUID,
    b_user_id UUID,
    created_at TIMESTAMP,
    PRIMARY KEY ((a_user_id, b_user_id))
);
