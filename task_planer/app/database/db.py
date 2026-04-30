#thư viện tương tác với sqlite
import sqlite3
#thư viện làm việc với đường dẫn
from pathlib import Path

DB_PATH = Path("data/app.db")

def get_connection():
    #kết nối tới local file hoặc tạo mới nếu không có
    return sqlite3.connect(DB_PATH)

def init_db():
    #tạo thư mục từ path
    DB_PATH.parent.mkdir(exist_ok=True)

    #tạo kết nối tới database để thực hiện thao tác dữ liệu (tạo table mới, insert, delete/remove)
    #trả về object với connect
    conn = get_connection()
    with open("app/database/schema.sql") as f:
        #các sql lệnh ngăn cách bởi dấu ;, nên nếu đọc file dưới dạng text nó sẽ tự parse và thực thi
        #các câu lệnh đó
        conn.executescript(f.read())
    #đóng kết nói giải phóng bộ nhớ RAM
    conn.close()