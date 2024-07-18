CREATE TABLE
    Users (
        id SERIAL PRIMARY KEY,
        display_name VARCHAR(50) NOT NULL,
        user_name VARCHAR(50) NOT NULL,
        user_email VARCHAR(200) NOT NULL,
        user_avatar BYTEA,
        user_password VARCHAR(200) NOT NULL,
        online_status INT NOT NULL DEFAULT 0,
        joined_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    );

CREATE INDEX idx_users_user_name ON Users(user_name);
CREATE INDEX idx_users_user_email ON Users(user_email);
CREATE INDEX idx_users_online_status ON Users(online_status);