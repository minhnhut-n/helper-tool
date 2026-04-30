CREATE TABLE IF NOT EXISTS tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    parent_id INTEGER,
    created_at TEXT,
    deadline TEXT,
    is_done INTEGER DEFAULT 0
);