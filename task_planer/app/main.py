import sys
from PySide6.QtWidgets import QApplication
from app.gui.main_window import MainWindow
from app.database.db import init_db

def run_app():
    init_db()

    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()

    sys.exit(app.exec())