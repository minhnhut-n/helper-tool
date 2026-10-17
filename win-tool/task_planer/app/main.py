import sys
from pathlib import Path
from PySide6.QtGui import QIcon
from PySide6.QtWidgets import QApplication
from app.gui.main_window import MainWindow
from app.database.db import init_db

def run_app():
    init_db()

    app = QApplication(sys.argv)
    if getattr(sys, "frozen", False) and hasattr(sys, "_MEIPASS"):
        app_root = Path(sys._MEIPASS)
    else:
        app_root = Path(__file__).resolve().parents[1]

    icon_path = app_root / "docs" / "image" / "smiling_face.ico"
    app_icon = QIcon(str(icon_path))
    app.setWindowIcon(app_icon)

    window = MainWindow()
    window.setWindowIcon(app_icon)
    window.show()

    sys.exit(app.exec())
