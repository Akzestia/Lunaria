CREATE KEYSPACE IF NOT EXISTS lunnaria_service
WITH replication = {
    'class': 'SimpleStrategy',
    'replication_factor': 1
};

USE lunnaria_service;

CREATE TABLE IF NOT EXISTS Messages (
    id UUID PRIMARY KEY,
    sender_id UUID,
    receiver_id UUID,
    text_content TEXT,
    byte_content BLOB,
    created_at TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_messages_sender_id ON Messages(sender_id);
CREATE INDEX IF NOT EXISTS idx_messages_receiver_id ON Messages(receiver_id);
CREATE INDEX IF NOT EXISTS idx_messages_created_at ON Messages(created_at);
