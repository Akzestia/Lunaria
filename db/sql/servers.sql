CREATE TABLE if not exists
    Server (
        id SERIAL NOT NULL PRIMARY KEY,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    );
