CREATE TABLE
    Messages (
        id SERIAL PRIMARY KEY,
        sender_id INT NOT NULL,
        receiver_id INT NOT NULL,
        text_content VARCHAR(2000),
        byte_content BYTEA,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY (sender_id) REFERENCES Users (id),
        FOREIGN KEY (receiver_id) REFERENCES Users (id)
    );

CREATE INDEX idx_messages_sender_id ON Messages(sender_id);
CREATE INDEX idx_messages_receiver_id ON Messages(receiver_id);
CREATE INDEX idx_messages_created_at ON Messages(created_at);