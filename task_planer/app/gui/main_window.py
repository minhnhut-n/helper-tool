# bộ công cụ open source từ QT
from PySide6.QtWidgets import (
    QMainWindow, QWidget, QVBoxLayout,
    QPushButton, QLabel, QListWidget
)

#thêm hàm từ các file khác vào chương trình này
from app.services.task_service import get_progress
from app.repositories.task_repo import get_all_tasks, create_task

#quản lý gui dưới dạng một class, và có các methods
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        #titlee của box
        self.setWindowTitle("Task Planner")
        #bố cục dạng dọc của QT + vertical
        self.layout = QVBoxLayout()

        self.progress_label = QLabel("Progress: 0%")
        self.layout.addWidget(self.progress_label)

        #task list là widget
        self.task_list = QListWidget()
        self.layout.addWidget(self.task_list)

        #tạo button
        self.add_btn = QPushButton("Add Task")
        self.add_btn.clicked.connect(self.add_task)
        self.layout.addWidget(self.add_btn)

        #final biến chưa tất cả thông tin của một window của tool
        container = QWidget()
        container.setLayout(self.layout)
        self.setCentralWidget(container)

        self.refresh()

    def refresh(self):
        #xóa các task hiện có
        self.task_list.clear()

        #lấy lại thông tin các task (để update nếu thêm hoặc xóa đi một task)
        tasks = get_all_tasks()
        for t in tasks:
            self.task_list.addItem(f"{t[0]} - {t[1]}")

        #lấy tiến độ hiện tại của các task
        progress = get_progress()
        self.progress_label.setText(f"Progress: {progress}%")

    def add_task(self):
        create_task("New Task")
        #tương tự sẽ refresh
        self.refresh()