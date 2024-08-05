CREATE TABLE if not exists
    Encrypt_keys (
        id SERIAL NOT NULL PRIMARY KEY,
        _key VARCHAR(32) NOT NULL,
        _user_id INT NOT NULL,
        FOREIGN KEY (_user_id) REFERENCES Users (id),
        CONSTRAINT unique_user_id UNIQUE (_user_id)
    );

CREATE INDEX if not exists idx_user_id ON Encrypt_keys(_user_id);
