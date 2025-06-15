from typing import Optional

from maya import OpenMayaUI
from maya import cmds, mel

from .Qt import QtWidgets, QtCore, QtGui, QtCompat

from pathlib import Path


def get_module_path() -> Path:
    """
    Returns the path to the module directory.
    """
    module_name = "DRPForMaya"
    try:
        return Path(cmds.getModulePath(moduleName=f"{module_name}"))
    except RuntimeError:
        raise RuntimeError(f"Module {module_name} not found. Make sure it was installed correctly.")


def add_menu():
    maya_window = mel.eval('$tempMelVar=$gMainWindow')
    cmds.menu("RichPresenceMenu", label="Rich Presence", tearOff=True, parent=maya_window)
    cmds.menuItem(label="Settings", command=lambda *args: show())


def remove_menu():
    cmds.deleteUI("RichPresenceMenu", menu=True)


class TypedSettings(QtCore.QSettings):

    _CONFIG_PATH = get_module_path() / "config/config.ini"

    def __init__(self):
        super().__init__(self._CONFIG_PATH.as_posix(), QtCore.QSettings.Format.IniFormat)

    def get(self, setting: str):
        return bool(self.value(setting, False))

    def set(self, setting: str, value: bool):
        assert isinstance(value, bool)
        self.setValue(setting, value)


class RichPresenceUI(QtWidgets.QDialog):

    def __init__(self):
        parent = QtCompat.wrapInstance(int(OpenMayaUI.MQtUtil.mainWindow()), QtWidgets.QWidget)
        super().__init__(parent)
        with open(Path(__file__).parent / "style.qss", "r") as f:
            stylesheet = f.read()
            self.setStyleSheet(stylesheet)

        self.setWindowTitle("Rich Presence For Maya Settings")

        root_layout = QtWidgets.QVBoxLayout(self)
        root_layout.setContentsMargins(0, 0, 0, 0)

        bar = QtWidgets.QMenuBar()
        menu = QtWidgets.QMenu("File")
        menu.setStyleSheet(stylesheet)
        action = QtWidgets.QAction("Reset to defaults", self)
        action.triggered.connect(self.reset_options)
        menu.addAction(action)
        bar.addMenu(menu)
        root_layout.addWidget(bar)

        layout = QtWidgets.QVBoxLayout()
        layout.setContentsMargins(11, 0, 11, 11)
        root_layout.addLayout(layout)

        layout.addWidget(SettingsWidget(), QtCore.Qt.AlignmentFlag.AlignCenter)

        button_layout = QtWidgets.QHBoxLayout()
        layout.addLayout(button_layout)
        self._save_btn = QtWidgets.QPushButton("Save")
        self._save_btn.clicked.connect(self.save)
        button_layout.addWidget(self._save_btn)

        self._cancel_btn = QtWidgets.QPushButton("Cancel")
        self._cancel_btn.clicked.connect(self.close)
        button_layout.addWidget(self._cancel_btn)

    def save(self):
        settings = TypedSettings()
        arguments = {}
        for entry in self.findChildren(SettingsEntry):
            settings.set(entry.objectName(), entry.is_checked)
            arguments[entry.command] = entry.is_checked

        cmds.richPresence(**arguments)

        self.close()

    def reset_options(self):
        for entry in self.findChildren(SettingsEntry):
            entry.set_checked(True)


class SettingsWidget(QtWidgets.QWidget):

    def __init__(self):
        super().__init__()
        layout = QtWidgets.QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)

        layout.addWidget(SettingsEntry("Details", "Show or hide the scene name.", "displayScene"))
        layout.addWidget(Separator())
        layout.addWidget(SettingsEntry("State", "Show or hide the project name.", "displayProject"))

        layout.addWidget(Separator())
        layout.addWidget(SettingsEntry("Reset Time With Scene", "Reset the timer when a new scene is opened.", "resetTimeOnChange"))


class Separator(QtWidgets.QFrame):

    def __init__(self):
        super().__init__()
        self.setFrameShape(QtWidgets.QFrame.HLine)
        self.setFrameShadow(QtWidgets.QFrame.Plain)


class SettingsEntry(QtWidgets.QWidget):

    def __init__(self, label: str, desc:str, command: str):
        super().__init__()
        assert command
        self._command = command

        self.setObjectName(label.replace(" ", "_").lower())
        layout = QtWidgets.QGridLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)

        self._label = QtWidgets.QLabel(f"{label}")
        self._label.setStyleSheet("QLabel { font-weight: bold; font-size: 10pt; }")
        layout.addWidget(self._label, 0, 0, 1, 1)

        self._desc = QtWidgets.QLabel(desc)
        layout.addWidget(self._desc, 1, 0, 1, 1)

        layout.addItem(QtWidgets.QSpacerItem(100, 0, QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Minimum), 0, 1, 1, 2)
        layout.setColumnStretch(1, True)

        self._checkbox = AnimatedCheckBox(TypedSettings().get(self.objectName()))
        layout.addWidget(self._checkbox, 0, 2, 2, 1)

    @property
    def name(self):
        return self._label.text()

    @property
    def is_checked(self):
        return self._checkbox.isChecked()

    def set_checked(self, checked):
        self._checkbox.setChecked(checked)

    @property
    def command(self):
        return self._command

class AnimatedCheckBox(QtWidgets.QCheckBox):

    _ACTIVE_COLOR = QtGui.QColor(88, 101, 242)
    _INACTIVE_COLOR = QtGui.QColor(77, 80, 91)
    _BUTTON_COLOR = QtGui.QColor("white")

    _BUTTON_MARGIN = 6

    def __init__(self, checked = False):
        super().__init__()
        self.setChecked(checked)

        aspect_ratio = 2
        width = 45
        height = round(width / aspect_ratio)
        self._size = QtCore.QSize(width, height)

        self.setSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Fixed)
        self.setMinimumSize(QtCore.QSize(width, height))

        self._background_path: Optional[QtGui.QPainterPath] = None
        self.init_painter_paths()

        self._handle_position = self.isChecked()

        self._animation = QtCore.QPropertyAnimation(self, b"handle_position")
        self._animation.setEasingCurve(QtCore.QEasingCurve.Type.InQuad)
        self._animation.setDuration(100)

        self.toggled.connect(self._start_anim)

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        painter.setRenderHint(QtGui.QPainter.RenderHint.Antialiasing)
        painter.fillPath(self._background_path, self.lerp_color(self._INACTIVE_COLOR, self._ACTIVE_COLOR, self._handle_position))

        painter.setBrush(self._BUTTON_COLOR)
        painter.setPen(QtCore.Qt.NoPen)
        diameter = self._size.height() - self._BUTTON_MARGIN
        margin = round(self._BUTTON_MARGIN / 2)

        end_pos = self._size.width() - diameter - margin
        x_offset = round(self.lerp(margin, end_pos, self._handle_position))

        rect = QtCore.QRect(x_offset, margin, diameter, diameter)
        painter.drawEllipse(rect)

    @staticmethod
    def lerp(start, end, t):
        return start + (end - start) * t

    @staticmethod
    def lerp_color(start, end, t):
        return QtGui.QColor(
            int(start.red() + (end.red() - start.red()) * t),
            int(start.green() + (end.green() - start.green()) * t),
            int(start.blue() + (end.blue() - start.blue()) * t)
        )

    def init_painter_paths(self):
        rect = QtCore.QRect(0, 0, self._size.width(), self._size.height())
        radius = rect.height() / 2
        path = QtGui.QPainterPath()
        path.moveTo(rect.center())
        path.addRoundedRect(rect, radius, radius)

        self._background_path = path

    @QtCore.Property(float)
    def handle_position(self):
        return self._handle_position

    @handle_position.setter
    def handle_position(self, handle_position: float):
        self._handle_position = handle_position
        self.update()

    def mouseReleaseEvent(self, event):
        if event.button() == QtCore.Qt.MouseButton.LeftButton:
            if self._background_path.contains(event.position()):
                self.toggle()

    def _start_anim(self):
        self._animation.stop()
        self._animation.setEndValue(float(self.isChecked()))
        self._animation.start()

    def enterEvent(self, event):
        super().enterEvent(event)
        self.setCursor(QtCore.Qt.CursorShape.PointingHandCursor)

    def leaveEvent(self, event):
        super().leaveEvent(event)
        self.setCursor(QtCore.Qt.CursorShape.ArrowCursor)


def show():
    dialog = RichPresenceUI()
    dialog.exec_()
    return dialog
