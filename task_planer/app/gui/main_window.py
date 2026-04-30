# bộ công cụ open source từ QT
from PySide6.QtCore import Qt
from PySide6.QtWidgets import (
    QMainWindow, QWidget, QVBoxLayout,
    QPushButton, QLabel, QListWidget, QListWidgetItem, QInputDialog
)

#thêm hàm từ các file khác vào chương trình này
from app.services.task_service import get_progress, get_task_progress, set_done
from app.repositories.task_repo import get_all_tasks, create_task, delete_task

#quản lý gui dưới dạng một class, và có các methods
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        #titlee của box
        self.setWindowTitle("Task Planner")
        #bố cục dạng dọc của QT + vertical
        self.layout = QVBoxLayout()
        self._is_refreshing = False

        self.progress_label = QLabel("Progress: 0%")
        self.layout.addWidget(self.progress_label)

        #task list là widget
        self.task_list = QListWidget()
        self.task_list.itemChanged.connect(self.on_item_changed)
        self.layout.addWidget(self.task_list)

        #tạo button
        self.add_btn = QPushButton("Add Task")
        self.add_btn.clicked.connect(self.add_task)
        self.layout.addWidget(self.add_btn)

        self.add_subtask_btn = QPushButton("Add Subtask")
        self.add_subtask_btn.clicked.connect(self.add_subtask)
        self.layout.addWidget(self.add_subtask_btn)

        self.delete_btn = QPushButton("Delete Task")
        self.delete_btn.clicked.connect(self.delete_selected_task)
        self.layout.addWidget(self.delete_btn)

        #final biến chưa tất cả thông tin của một window của tool
        container = QWidget()
        container.setLayout(self.layout)
        self.setCentralWidget(container)

        self.refresh()

    def refresh(self):
        #xóa các task hiện có
        self._is_refreshing = True
        self.task_list.clear()

        #lấy lại thông tin các task (để update nếu thêm hoặc xóa đi một task)
        tasks = get_all_tasks()
        by_parent = {}
        for task in tasks:
            by_parent.setdefault(task[2], []).append(task)

        def add_items(parent_id=None, level=0, prefix=""):
            siblings = by_parent.get(parent_id, [])
            for idx, task in enumerate(siblings, start=1):
                display_index = f"{prefix}.{idx}" if prefix else str(idx)
                task_progress = get_task_progress(task[0], tasks)
                item = QListWidgetItem(
                    f"{'  ' * level}{display_index} - {task[1]} ({task_progress}%)"
                )
                item.setData(Qt.ItemDataRole.UserRole, task[0])
                item.setFlags(item.flags() | Qt.ItemFlag.ItemIsUserCheckable)
                item.setCheckState(
                    Qt.CheckState.Checked if task[5] else Qt.CheckState.Unchecked
                )
                self.task_list.addItem(item)
                add_items(task[0], level + 1, display_index)

        add_items()

        #lấy tiến độ hiện tại của các task
        progress = get_progress()
        self.progress_label.setText(f"Progress: {progress}%")
        self._is_refreshing = False

    def add_task(self):
        title, ok = QInputDialog.getText(self, "Add Task", "Task title:")
        if not ok or not title.strip():
            return
        create_task(title.strip())
        #tương tự sẽ refresh
        self.refresh()

    def add_subtask(self):
        current = self.task_list.currentItem()
        if current is None:
            return
        parent_id = current.data(Qt.ItemDataRole.UserRole)
        title, ok = QInputDialog.getText(self, "Add Subtask", "Subtask title:")
        if not ok or not title.strip():
            return
        create_task(title.strip(), parent_id=parent_id)
        self.refresh()

    def delete_selected_task(self):
        current = self.task_list.currentItem()
        if current is None:
            return
        task_id = current.data(Qt.ItemDataRole.UserRole)
        delete_task(task_id)
        self.refresh()

    def on_item_changed(self, item):
        if self._is_refreshing:
            return
        task_id = item.data(Qt.ItemDataRole.UserRole)
        is_done = item.checkState() == Qt.CheckState.Checked
        set_done(task_id, is_done)
        self.refresh()
