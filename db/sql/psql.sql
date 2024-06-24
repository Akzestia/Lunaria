-- Create a new database
CREATE DATABASE exampledb;

-- Connect to the new database
\c exampledb;

-- Create a table
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100),
    email VARCHAR(100) UNIQUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Insert some data
INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com');
INSERT INTO users (name, email) VALUES ('Bob', 'bob@example.com');

-- Query the data
SELECT * FROM users;

-- Update some data
UPDATE users SET email = 'alice@newdomain.com' WHERE name = 'Alice';

-- Delete some data
DELETE FROM users WHERE name = 'Bob';