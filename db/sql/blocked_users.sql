CREATE TABLE if not exists Blocked_users (
    id SERIAL NOT NULL PRIMARY KEY,
    _blocked_id INT NOT NULL,
    _blocker_id INT NOT NULL,
    FOREIGN KEY (_blocked_id) REFERENCES Users (id),
    FOREIGN KEY (_blocker_id) REFERENCES Users (id),
    blocked_from TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX if not exists idx_blocked_users_blocked_id ON Blocked_users(_blocked_id);
CREATE INDEX if not exists idx_blocked_users_blocker_id ON Blocked_users(_blocker_id);
