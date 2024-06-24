CREATE TABLE Users
(
    id SERIAL PRIMARY KEY,
    user_name VARCHAR(50) NOT NULL,
    user_email VARCHAR(200) NOT NULL,
    user_avatar BYTEA,
    user_password VARCHAR(200) NOT NULL
)
