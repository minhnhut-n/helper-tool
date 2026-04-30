#thư viện tương tác với sqlite
import os
import sqlite3
import sys
#thư viện làm việc với đường dẫn
from pathlib import Path

if getattr(sys, "frozen", False):
    BASE_DIR = Path(os.getenv("LOCALAPPDATA", Path.home())) / "TaskPlanner"
else:
    BASE_DIR = Path(__file__).resolve().parents[2]

DB_PATH = BASE_DIR / "data" / "app.db"
SCHEMA_PATH = Path(__file__).resolve().parent / "schema.sql"
DEFAULT_SCHEMA = """
CREATE TABLE IF NOT EXISTS tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    parent_id INTEGER,
    created_at TEXT,
    deadline TEXT,
    is_done INTEGER DEFAULT 0
);
"""

def _load_schema_sql():
    if SCHEMA_PATH.exists():
        return SCHEMA_PATH.read_text(encoding="utf-8")
    return DEFAULT_SCHEMA

def get_connection():
    #kết nối tới local file hoặc tạo mới nếu không có
    return sqlite3.connect(DB_PATH)

def init_db():
    #tạo thư mục từ path
    DB_PATH.parent.mkdir(parents=True, exist_ok=True)

    #tạo kết nối tới database để thực hiện thao tác dữ liệu (tạo table mới, insert, delete/remove)
    #trả về object với connect
    conn = get_connection()
    #các sql lệnh ngăn cách bởi dấu ;, nên nếu đọc file dưới dạng text nó sẽ tự parse và thực thi
    #các câu lệnh đó
    conn.executescript(_load_schema_sql())
    #đóng kết nói giải phóng bộ nhớ RAM
    conn.close()
