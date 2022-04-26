from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import *

class NoiseControlWidget(QGroupBox):
    def __init__(self, config, parent=None):
        super(NoiseControlWidget, self).__init__(
            "Noise control", parent=parent)

        # Components.
        self.noise_enable_button = QPushButton("Enable")
        self.noise_enable_button.setCheckable(True)
        self.noise_enable_button.setFixedWidth(70)

        # Layout.
        self.main_layout = QVBoxLayout()
        self.main_layout.addWidget(self.noise_enable_button)
        self.main_layout.setAlignment(self.noise_enable_button, Qt.AlignCenter)

        self.setLayout(self.main_layout)

        self.set_config(config)

    def get_config(self):
        config = {}
        config["enabled"] = self.noise_enable_button.isChecked()
        return config

    def set_config(self, config):
        self.noise_enable_button.setChecked(config["enabled"])