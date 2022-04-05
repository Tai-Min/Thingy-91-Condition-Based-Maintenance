from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import *

class HeaterControlWidget(QGroupBox):
    def __init__(self, config, parent=None):
        super(HeaterControlWidget, self).__init__(
            "Heater control", parent=parent)

        # Components.
        self.heater_enable_button = QPushButton("Enable")
        self.heater_enable_button.setCheckable(True)
        self.heater_enable_button.setFixedWidth(70)

        # Layout.
        self.main_layout = QVBoxLayout()
        self.main_layout.addWidget(self.heater_enable_button)
        self.main_layout.setAlignment(self.heater_enable_button, Qt.AlignCenter)

        self.setLayout(self.main_layout)

        self.set_config(config)

    def get_config(self):
        config = {}
        config["enabled"] = self.heater_enable_button.isChecked()
        return config

    def set_config(self, config):
        self.heater_enable_button.setChecked(config["enabled"])