CREATE TABLE users (
    id_user SERIAL PRIMARY KEY,
    name VARCHAR(50),
    email VARCHAR(100) UNIQUE NOT NULL,
    password TEXT NOT NULL
);

CREATE TABLE channels (
    id_channel SERIAL PRIMARY KEY,
    name VARCHAR(50) NOT NULL
);

CREATE TABLE permissions (
    user_id INTEGER REFERENCES users(id_user) ON DELETE CASCADE,
    channel_id INTEGER REFERENCES channels(id_channel) ON DELETE CASCADE,
    droit VARCHAR(20),
    PRIMARY KEY (user_id,channel_id)
);

CREATE TABLE messages (
    id_message SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(id_user) ON DELETE SET NULL,
    channel_id INTEGER REFERENCES channels(id_channel) ON DELETE CASCADE,
    content TEXT NOT NULL,
    timestamp TIMESTAMP DEFAULT NOW()
);

CREATE TABLE reactions (
    id_reaction SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(id_user) ON DELETE CASCADE,
    message_id INTEGER REFERENCES messages(id_message) ON DELETE CASCADE,
    emoji VARCHAR(8)
);