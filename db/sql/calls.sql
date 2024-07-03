CREATE TABLE
    Calls (
        id SERIAL NOT NULL PRIMARY KEY,
        _type INT NOT NULL DEFAULT 0,
        _sender_id INT NOT NULL,
        _receiver_id INT NOT NULL,
        _duration INT NOT NULL DEFAULT 0,

        FOREIGN KEY (_sender_id) REFERENCES Users(id),
        FOREIGN KEY (_receiver_id) REFERENCES Users(id),

        made_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )

CREATE INDEX idx_calls_sender_id ON Calls(_sender_id);
CREATE INDEX idx_calls_receiver_id ON Calls(_receiver_id);
CREATE INDEX idx_calls_made_at ON Calls(made_at);