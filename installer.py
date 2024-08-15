import pathlib
import shutil
from maya import cmds
from maya.api import OpenMaya as om

try:
    from PySide2 import QtWidgets, QtGui
except ImportError:
    from PySide6 import QtWidgets, QtGui


GUI = None


# noinspection PyUnresolvedReferences
class Installer(QtWidgets.QDialog):

    def __init__(self):
        super().__init__()
        self.setWindowTitle("Discord Rich Presence Installer")
        self.setFixedWidth(300)
        self.path = pathlib.Path(__file__).parent
        layout = QtWidgets.QVBoxLayout(self)

        self.console = Console("Discord Rich Presence is not installed. Would you like to install it?")
        self.console.setReadOnly(True)
        layout.addWidget(self.console)

        self.progress = QtWidgets.QProgressBar()
        self.progress.setRange(0, 100)
        layout.addWidget(self.progress)

        button_layout = QtWidgets.QHBoxLayout()

        self.all_versions = QtWidgets.QCheckBox("All Versions")
        button_layout.addWidget(self.all_versions)

        install_btn = QtWidgets.QPushButton("Install")
        install_btn.clicked.connect(self.install)
        button_layout.addWidget(install_btn)

        uninstall_btn = QtWidgets.QPushButton("Uninstall")
        uninstall_btn.clicked.connect(self.uninstall)
        button_layout.addWidget(uninstall_btn)

        layout.addLayout(button_layout)

    def install(self):
        version_dir = self.path / "versions"
        maya_folder = pathlib.Path(cmds.internalVar(userAppDir=True))
        maya_versions = [x.name for x in maya_folder.iterdir() if x.is_dir() and x.name.isdigit()]
        if self.all_versions.isChecked():
            response = QtWidgets.QMessageBox.question(self, "Install All Versions",
                                                      "Are you sure you want to install Discord Rich Presence for all "
                                                      "versions of Maya?",
                                                      QtWidgets.QMessageBox.Yes | QtWidgets.QMessageBox.No)
            if response == QtWidgets.QMessageBox.No:
                return
            versions_to_install = [version for version in version_dir.iterdir() if version.name in maya_versions]
        else:
            versions_to_install = [version_dir / cmds.about(majorVersion=True)]
            if not versions_to_install[0].is_dir():
                om.MGlobal.displayError("Discord Rich Presence is not available for this version of Maya.")
                return
        self.console.log("Installing Discord Rich Presence...", clear=True)
        increment = 100 / len(versions_to_install)
        for i, version in enumerate(versions_to_install):
            # stubs are expecting ints, but floats work just fine
            # noinspection PyTypeChecker
            self.progress.setValue(increment * (i + 1))
            if cmds.pluginInfo("RichPresence", q=True, loaded=True):
                cmds.unloadPlugin("RichPresence")
            plugin_path = version / "RichPresence.mll"
            # make sure plugin path exists
            test = maya_folder / version.name / "plug-ins"
            print(test)
            test.mkdir(parents=True, exist_ok=True)
            try:
                shutil.copyfile(plugin_path, maya_folder / version.name / "plug-ins/RichPresence.mll")
            except PermissionError as e:
                self.console.add_divider()
                self.console.error(f"Failed to install Discord Rich Presence for Maya {version.name}: {e}")
            else:
                self.console.add_divider()
                self.console.log(f"Successfully installed Discord Rich Presence for Maya {version.name}")
        self.console.add_divider()
        self.console.log("Finished Installing Discord Rich Presence, Make sure to enable it in the Plug-in Manager.")

    def uninstall(self):
        maya_folder = pathlib.Path(cmds.internalVar(userAppDir=True))
        if self.all_versions.isChecked():
            response = QtWidgets.QMessageBox.question(self, "Uninstall All Versions",
                                                      "Are you sure you want to uninstall Discord Rich Presence for all "
                                                      "versions of Maya?",
                                                      QtWidgets.QMessageBox.Yes | QtWidgets.QMessageBox.No)
            if response == QtWidgets.QMessageBox.No:
                return
            versions_to_uninstall = [x for x in maya_folder.iterdir() if x.is_dir() and x.name.isdigit()]
        else:
            versions_to_uninstall = [maya_folder / cmds.about(majorVersion=True)]
        self.console.log("Uninstalling Discord Rich Presence...", clear=True)
        increment = 100 / len(versions_to_uninstall)
        if cmds.pluginInfo("RichPresence", q=True, loaded=True):
            cmds.unloadPlugin("RichPresence")
        for i, version in enumerate(versions_to_uninstall):
            # stubs are expecting ints, but floats work just fine
            # noinspection PyTypeChecker
            self.progress.setValue(increment * (i + 1))
            try:
                (version / "plug-ins/RichPresence.mll").unlink()
            except PermissionError as e:
                self.console.add_divider()
                self.console.error(f"Failed to uninstall Discord Rich Presence for Maya {version.name}: {e}")
            except FileNotFoundError:
                continue
            else:
                self.console.add_divider()
                self.console.log(f"Successfully uninstalled Discord Rich Presence for Maya {version.name}")


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


def onMayaDroppedPythonFile(*_):
    gui = Installer()
    gui.exec_()
