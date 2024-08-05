CREATE TABLE if not exists
    Contacts (
        id SERIAL PRIMARY KEY,
        a_user_id INT NOT NULL,
        b_user_id INT NOT NULL,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY (a_user_id) REFERENCES Users (id),
        FOREIGN KEY (b_user_id) REFERENCES Users (id)
    );

CREATE INDEX if not exists idx_contacts_a_user_id ON Contacts(a_user_id);
CREATE INDEX if not exists idx_contacts_b_user_id ON Contacts(b_user_id);
