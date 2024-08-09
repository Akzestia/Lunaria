CREATE KEYSPACE IF NOT EXISTS lunnaria_service
WITH replication = {
    'class': 'SimpleStrategy',
    'replication_factor': 1
};

USE lunnaria_service;

CREATE TABLE IF NOT EXISTS Servers(
    id UUID,
    name TEXT,
    icon BLOB,
    banner BLOB,
    description TEXT,

    PRIMARY KEY((name)id)
);

CREATE TABLE IF NOT EXISTS UserServers(
    user_id UUID,
    server_id UUID,
    role TEXT,
    PRIMARY KEY((user_id)server_id)
);

CREATE TABLE IF NOT EXISTS Roles(
    id UUID,
    server_id UUID,
    name TEXT,
    color INT,
    permissions INT,
    position INT,
    PRIMARY KEY((server_id)id)
);

CREATE TABLE IF NOT EXISTS RoleMembers(
    role_id UUID,
    user_id UUID,
    PRIMARY KEY((role_id)user_id)
);

CREATE TABLE IF NOT EXISTS OwnerServers(
    owner_id UUID,
    server_id UUID,
    PRIMARY KEY((owner_id)server_id)
);

CREATE TABLE IF NOT EXISTS Channels(
    id UUID,
    server_id UUID,
    name TEXT,
    type TEXT,
    position INT,
    nsfw BOOLEAN,
    PRIMARY KEY((server_id)id)
);
