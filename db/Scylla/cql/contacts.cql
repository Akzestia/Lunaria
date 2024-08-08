CREATE KEYSPACE IF NOT EXISTS lunnaria_service
WITH replication = {
    'class': 'SimpleStrategy',
    'replication_factor': 1
};

USE lunnaria_service;

CREATE TABLE IF NOT EXISTS Contacts (
    id UUID PRIMARY KEY,
    a_user_id UUID,
    b_user_id UUID,
    created_at TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_contacts_a_user_id ON Contacts(a_user_id);
CREATE INDEX IF NOT EXISTS idx_contacts_b_user_id ON Contacts(b_user_id);
