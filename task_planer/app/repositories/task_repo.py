# trong file này có chứa các method để tương tác trực tiếp với data của DB, SQL

#lấy lại function get_connect từ db
from datetime import datetime

from app.database.db import get_connection

def create_task(title, parent_id=None, deadline=None):
    conn = get_connection()
    cursor = conn.cursor()

    #dịch ra là: chèn vào db có tên là task, các thông số sau...
    # ở trạng thái tạm thời (giống như status)
    cursor.execute("""
        INSERT INTO tasks (title, parent_id, created_at, deadline)
        VALUES (?, ?, ?, ?)
    """, (title, parent_id, datetime.now(), deadline))

    #lưu vĩnh viễn các thay đổi mới vào trong db
    conn.commit()
    task_id = cursor.lastrowid
    conn.close()
    return task_id

def get_all_tasks():
    conn = get_connection()
    cursor = conn.cursor()

    cursor.execute("SELECT * FROM tasks ORDER BY id ASC")
    #trả lại nội dung của dòng
    rows = cursor.fetchall()

    conn.close()
    return rows

def get_task(task_id):
    conn = get_connection()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM tasks WHERE id = ?", (task_id,))
    #trả về giá trị duy nhất 1 dòng nếu có kết quả
    #Một Tuple: Nếu tìm thấy dữ liệu (ví dụ: (1, 'Alice', 'Admin')).
    row = cursor.fetchone()
    conn.close()
    return row

def delete_task(task_id):
    conn = get_connection()
    cursor = conn.cursor()

    cursor.execute(
        """
        WITH RECURSIVE subtree(id) AS (
            SELECT id FROM tasks WHERE id = ?
            UNION ALL
            SELECT t.id
            FROM tasks t
            INNER JOIN subtree s ON t.parent_id = s.id
        )
        DELETE FROM tasks
        WHERE id IN (SELECT id FROM subtree)
        """,
        (task_id,),
    )
    conn.commit()
    conn.close()

def update_done(task_id, is_done):
    conn = get_connection()
    cursor = conn.cursor()

    cursor.execute(
        """
        WITH RECURSIVE subtree(id) AS (
            SELECT id FROM tasks WHERE id = ?
            UNION ALL
            SELECT t.id
            FROM tasks t
            INNER JOIN subtree s ON t.parent_id = s.id
        )
        UPDATE tasks
        SET is_done = ?
        WHERE id IN (SELECT id FROM subtree)
        """,
        (task_id, is_done),
    )

    conn.commit()
    conn.close()
