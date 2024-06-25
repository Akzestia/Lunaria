CREAET TABLE Blocked_users
(
    id SERIAL NOT NULL PRIMARY KEY,
    _blocked_id INT NOT NULL,
    _blocker_id INT NOT NULL,
    FOREIGN KEY (_blocked_id) REFERENCES Users(id),
    FOREIGN KEY (_blocker_id) REFERENCES Users(id)
)
