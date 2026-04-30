# trong file này có chứa các method để tương tác trực tiếp với data của DB, SQL

#lấy lại function get_connect từ db
from app.database.db import get_connection
#thêm ngày giờ cho ngày tạp/xóa task
from datetime import datetime

def create_task(title, parent_id=None, deadline=None):
    conn = get_connection()
    #lấy cursor hiện tại của chương trình khi mở database
    cursor = conn.cursor()

    #dịch ra là: chèn vào db có tên là task, các thông số sau...
    # ở trạng thái tạm thời (giống như status)
    cursor.execute("""
        INSERT INTO tasks (title, parent_id, created_at, deadline)
        VALUES (?, ?, ?, ?)
    """, (title, parent_id, datetime.now(), deadline))

    #lưu vĩnh viễn các thay đổi mới vào trong db
    conn.commit()
    #free
    conn.close()

def get_all_tasks():
    conn = get_connection()
    cursor = conn.cursor()

    cursor.execute("SELECT * FROM tasks")
    #gom tất cả các dòng về một danh sách (list) các tuple
    rows = cursor.fetchall()

    conn.close()
    return rows

def delete_task(task_id):
    conn = get_connection()
    cursor = conn.cursor()

    cursor.execute("DELETE FROM tasks WHERE id = ?", (task_id,))
    conn.commit()
    conn.close()

def update_done(task_id, is_done):
    conn = get_connection()
    cursor = conn.cursor()

    cursor.execute(
        "UPDATE tasks SET is_done = ? WHERE id = ?",
        (is_done, task_id)
    )

    conn.commit()
    conn.close()