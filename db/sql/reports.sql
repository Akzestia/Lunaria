CREATE TABLE
    Reports (
        id SERIAL NOT NULL PRIMARY KEY,
        _reported_id INT NOT NULL,
        _reporter_id INT NOT NULL,
        _report_reason_id INT NOT NULL DEFAULT 0,
        _report_reason VARCHAR(150),
        FOREIGN KEY (_reported_id) REFERENCES Users (id),
        FOREIGN KEY (_reporter_id) REFERENCES Users (id),
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    )