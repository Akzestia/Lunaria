CREATE KEYSPACE IF NOT EXISTS lunnaria_service
WITH replication = {
    'class': 'SimpleStrategy',
    'replication_factor': 1
};

USE lunnaria_service;

CREATE TABLE IF NOT EXISTS Messages (
    id UUID,
    sender_id UUID,
    receiver_id UUID,
    text_content TEXT,
    byte_content BLOB,
    created_at TIMESTAMP,

    PRIMARY KEY ((sender_id, receiver_id), created_at)
);
