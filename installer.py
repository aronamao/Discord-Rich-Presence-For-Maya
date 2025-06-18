import os
import pathlib
import shutil


try:
    from PySide2 import QtWidgets, QtGui
except ImportError:
    from PySide6 import QtWidgets, QtGui


GUI = None

module_name = "DRPForMaya"

module_template = """+ MAYAVERSION:{version} {module_name} Any {module_path}
plug-ins: plug-ins/{version}
scripts: scripts
PATH+:= bin
"""


# noinspection PyUnresolvedReferences
class Installer(QtWidgets.QDialog):

    def __init__(self):
        super().__init__()
        self.setWindowTitle("Discord Rich Presence Installer")
        self.setMinimumWidth(500)
        self.path = pathlib.Path(__file__).parent
        self.plugin_dir = self.path / "module/plug-ins"
        self.maya_module_dir = pathlib.Path(os.path.expanduser('~/Documents')) / "maya/modules"
        self.target_dir = self.maya_module_dir / module_name

        layout = QtWidgets.QVBoxLayout(self)
        self.console = Console("Installing Discord Rich Presence for Maya...")
        layout.addWidget(self.console)

        btn = QtWidgets.QPushButton("Install")
        btn.clicked.connect(self.install)
        layout.addWidget(btn)

    def install(self):
        final_module = ""

        versions = list(self.find_versions())
        for version in versions:
            if final_module:
                final_module += "\n"
            final_module += self.format_template(version)

        if not self.maya_module_dir.exists():
            self.maya_module_dir.mkdir(parents=True, exist_ok=True)

        with open(self.maya_module_dir / f"{module_name}.mod", 'w') as f:
            f.write(final_module)

        try:
            shutil.copytree(self.path / "module", self.target_dir, dirs_exist_ok=True)
        except shutil.Error as e:
            self.console.error(f"Something went wrong while attempting to install.\nClose Maya and delete {self.target_dir}", clear=True)
            raise e

        self.console.log(f"Installed Discord Rich Presence for Maya for: {versions}", clear=True)
        self.console.add_divider()
        self.console.log("Please restart Maya to apply changes.")
        self.console.add_divider()
        self.console.log(f"To uninstall, simply delete {module_name}.mod and the {module_name} directory from \n{self.target_dir.parent}")

    def find_versions(self):
        for version in self.plugin_dir.iterdir():
            if version.is_dir():
                yield version.stem

    def format_template(self, version):
        module = module_template.format(version=version,
                                        module_path=self.target_dir,
                                        module_name=module_name)
        return module


class Console(QtWidgets.QPlainTextEdit):

    def __init__(self, text):
        super().__init__(text)
        self.setReadOnly(True)

    def log(self, text, clear=False):
        if clear:
            self.clear()
        self.appendPlainText(text)

    def error(self, text, clear=False):
        tf = self.currentCharFormat()
        tf.setForeground(QtGui.QColor("red"))
        self.setCurrentCharFormat(tf)
        self.log(text, clear)
        tf.setForeground(QtGui.QColor("white"))
        self.setCurrentCharFormat(tf)

    def warning(self, text, clear=False):
        tf = self.currentCharFormat()
        tf.setForeground(QtGui.QColor("yellow"))
        self.setCurrentCharFormat(tf)
        self.log(text, clear)
        tf.setForeground(QtGui.QColor("white"))
        self.setCurrentCharFormat(tf)

    def add_divider(self):
        self.appendPlainText("───────────────────────────────")


# noinspection PyPep8Naming
def onMayaDroppedPythonFile(*_):
    gui = Installer()
    gui.exec_()
