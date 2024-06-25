CREATE TABLE
    Contacts (
        id SERIAL PRIMARY KEY,
        a_user_id INT NOT NULL,
        b_user_id INT NOT NULL,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY (a_user_id) REFERENCES Users (id),
        FOREIGN KEY (b_user_id) REFERENCES Users (id),
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    )