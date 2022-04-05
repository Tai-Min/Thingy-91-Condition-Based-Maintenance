from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import *

class ImpactControlWidget(QGroupBox):
    def __init__(self, config, parent=None):
        super(ImpactControlWidget, self).__init__(
            "Impact control", parent=parent)

        # Components.
        self.motor_enable_fixed_button = QPushButton("Enable fixed")
        self.motor_enable_fixed_button.setCheckable(True)
        self.motor_enable_fixed_button.setFixedWidth(90)

        self.motor_enable_random_button = QPushButton("Enable random")
        self.motor_enable_random_button.setCheckable(True)
        self.motor_enable_random_button.setFixedWidth(90)

        self.motor_shock_period_label = QLabel("Motor shock period:")
        self.motor_shock_period_slider = QSlider(Qt.Horizontal)
        self.motor_shock_period_slider.setMinimum(100)
        self.motor_shock_period_slider.setMaximum(1000)
        self.motor_shock_period_value = QLabel(str(config["motor_shock_period"]) + " [ms]")
        self.motor_shock_period_value.setFixedWidth(40)

        self.motor_shock_filling_label = QLabel("Motor shock filling:")
        self.motor_shock_filling_slider = QSlider(Qt.Horizontal)
        self.motor_shock_filling_slider.setMinimum(10)
        self.motor_shock_filling_slider.setMaximum(100)
        self.motor_shock_filling_value = QLabel(str(config["motor_shock_filling"]) + " [%]")
        self.motor_shock_filling_value.setFixedWidth(40)

        self.load_enable_fixed_button = QPushButton("Enable fixed")
        self.load_enable_fixed_button.setCheckable(True)
        self.load_enable_fixed_button.setFixedWidth(90)

        self.load_enable_random_button = QPushButton("Enable random")
        self.load_enable_random_button.setCheckable(True)
        self.load_enable_random_button.setFixedWidth(90)

        self.load_shock_period_label = QLabel("Load shock period:")
        self.load_shock_period_slider = QSlider(Qt.Horizontal)
        self.load_shock_period_slider.setMinimum(100)
        self.load_shock_period_slider.setMaximum(1000)
        self.load_shock_period_value = QLabel(str(config["load_shock_period"]) + " [ms]")
        self.load_shock_period_value.setFixedWidth(40)

        self.load_shock_filling_label = QLabel("Load shock filling:")
        self.load_shock_filling_slider = QSlider(Qt.Horizontal)
        self.load_shock_filling_slider.setMinimum(10)
        self.load_shock_filling_slider.setMaximum(100)
        self.load_shock_filling_value = QLabel(str(config["load_shock_filling"]) + " [%]")
        self.load_shock_filling_value.setFixedWidth(40)

        # Layouts.
        self.motor_button_layout = QHBoxLayout()
        self.motor_button_layout.addWidget(self.motor_enable_fixed_button)
        self.motor_button_layout.addWidget(self.motor_enable_random_button)

        self.motor_slider_layout = QGridLayout()

        self.motor_slider_layout.addWidget(self.motor_shock_period_label, 0, 0)
        self.motor_slider_layout.addWidget(self.motor_shock_period_slider, 0, 1)
        self.motor_slider_layout.addWidget(self.motor_shock_period_value, 0, 2)

        self.motor_slider_layout.addWidget(self.motor_shock_filling_label, 1, 0)
        self.motor_slider_layout.addWidget(self.motor_shock_filling_slider, 1, 1)
        self.motor_slider_layout.addWidget(self.motor_shock_filling_value, 1, 2)

        self.load_button_layout = QHBoxLayout()
        self.load_button_layout.addWidget(self.load_enable_fixed_button)
        self.load_button_layout.addWidget(self.load_enable_random_button)

        self.load_slider_layout = QGridLayout()

        self.load_slider_layout.addWidget(self.load_shock_period_label, 3, 0)
        self.load_slider_layout.addWidget(self.load_shock_period_slider, 3, 1)
        self.load_slider_layout.addWidget(self.load_shock_period_value, 3, 2)

        self.load_slider_layout.addWidget(self.load_shock_filling_label, 4, 0)
        self.load_slider_layout.addWidget(self.load_shock_filling_slider, 4, 1)
        self.load_slider_layout.addWidget(self.load_shock_filling_value, 4, 2)

        self.main_layout = QVBoxLayout()
        self.main_layout.addLayout(self.motor_button_layout)
        self.main_layout.addLayout(self.motor_slider_layout)
        self.main_layout.addLayout(self.load_button_layout)
        self.main_layout.addLayout(self.load_slider_layout)

        self.setLayout(self.main_layout)

        # Connections.
        self.motor_enable_fixed_button.toggled.connect(self.toggle_motor_fixed_controls)
        self.motor_enable_random_button.toggled.connect(self.toggle_motor_random_controls)

        self.motor_shock_period_slider.valueChanged.connect(
            lambda val: self.motor_shock_period_value.setText(str(val) + " [ms]"))
        self.motor_shock_filling_slider.valueChanged.connect(
            lambda val: self.motor_shock_filling_value.setText(str(val) + " [%]"))

        self.load_enable_fixed_button.toggled.connect(self.toggle_load_fixed_controls)
        self.load_enable_random_button.toggled.connect(self.toggle_load_random_controls)
        
        self.load_shock_period_slider.valueChanged.connect(
            lambda val: self.load_shock_period_value.setText(str(val) + " [ms]"))
        self.load_shock_filling_slider.valueChanged.connect(
            lambda val: self.load_shock_filling_value.setText(str(val) + " [%]"))

        self.toggle_motor_fixed_controls(False)
        self.toggle_motor_random_controls(False)
        self.toggle_load_fixed_controls(False)
        self.toggle_load_random_controls(False)
        self.set_config(config)
        
    def toggle_motor_fixed_controls(self, enabled):
        self.motor_enable_random_button.setEnabled(not enabled)
        self.motor_shock_period_slider.setEnabled(enabled)
        self.motor_shock_filling_slider.setEnabled(enabled)

    def toggle_motor_random_controls(self, enabled):
        self.motor_enable_fixed_button.setEnabled(not enabled)

    def toggle_load_fixed_controls(self, enabled):
        self.load_enable_random_button.setEnabled(not enabled)
        self.load_shock_period_slider.setEnabled(enabled)
        self.load_shock_filling_slider.setEnabled(enabled)

    def toggle_load_random_controls(self, enabled):
        self.load_enable_fixed_button.setEnabled(not enabled)

    def get_config(self):
        config = {}
        config["motor_enable_fixed"] = self.motor_enable_fixed_button.isChecked()
        config["motor_enable_random"] = self.motor_enable_random_button.isChecked()
        config["motor_shock_period"] = self.motor_shock_period_slider.value()
        config["motor_shock_filling"] = self.motor_shock_filling_slider.value()
        config["load_enable_fixed"] = self.load_enable_fixed_button.isChecked()
        config["load_enable_random"] = self.load_enable_random_button.isChecked()
        config["load_shock_period"] = self.load_shock_period_slider.value()
        config["load_shock_filling"] = self.load_shock_filling_slider.value()
        return config

    def set_config(self, config):
        if config["motor_enable_fixed"] == True and config["motor_enable_random"] == True:
            raise ValueError("Enable fixed and Enable random can't be both true")
        self.motor_enable_fixed_button.setChecked(config["motor_enable_fixed"])
        self.motor_enable_random_button.setChecked(config["motor_enable_random"])

        if config["motor_shock_period"] > 1000 or config["motor_shock_period"] < 100:
            raise ValueError("Motor shock period must be between 0 and 1000 [ms]")
        self.motor_shock_period_slider.setValue(config["motor_shock_period"])

        if config["motor_shock_filling"] > 100 or config["motor_shock_filling"] < 10:
            raise ValueError("Motor shock filling must be between 0 and 100 [%]")
        self.motor_shock_filling_slider.setValue(config["motor_shock_filling"])

        if config["load_enable_fixed"] == True and config["load_enable_random"] == True:
            raise ValueError("Enable fixed and Enable random can't be both true")
        self.load_enable_fixed_button.setChecked(config["load_enable_fixed"])
        self.load_enable_random_button.setChecked(config["load_enable_random"])

        if config["load_shock_period"] > 1000 or config["load_shock_period"] < 100:
            raise ValueError("Load shock period must be between 0 and 1000 [ms]")
        self.load_shock_period_slider.setValue(config["load_shock_period"])

        if config["load_shock_filling"] > 100 or config["load_shock_filling"] < 10:
            raise ValueError("Load shock filling must be between 0 and 100 [%]")
        self.load_shock_filling_slider.setValue(config["load_shock_filling"])