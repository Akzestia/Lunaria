CREATE TABLE
    Calls (
        id SERIAL NOT NULL PRIMARY KEY,
        _type INT NOT NULL DEFAULT 0,
        _sender_id INT NOT NULL,
        _receiver_id INT NOT NULL,

        FOREIGN KEY (_sender_id) REFERENCES Users(id),
        FOREIGN KEY (_receiver_id) REFERENCES Users(id),

        made_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )