from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import *


class MotorControlWidget(QGroupBox):
    def __init__(self, config, parent=None):
        super(MotorControlWidget, self).__init__(
            "Motor control", parent=parent)

        # Components.
        self.motor_speed_label = QLabel("Motor PWM:")
        self.motor_speed_slider = QSlider(Qt.Horizontal)
        self.motor_speed_slider.setMinimum(-255)
        self.motor_speed_slider.setMaximum(255)
        self.motor_speed_value = QLabel(str(config["motor_pwm"]))
        self.motor_speed_value.setFixedWidth(30)

        self.load_speed_label = QLabel("Load PWM:")
        self.load_speed_slider = QSlider(Qt.Horizontal)
        self.load_speed_slider.setMinimum(-255)
        self.load_speed_slider.setMaximum(255)
        self.load_speed_value = QLabel(str(config["load_pwm"]))
        self.load_speed_value.setFixedWidth(30)

        # Layouts.
        self.slider_layout = QGridLayout()
        self.slider_layout.addWidget(self.motor_speed_label, 0, 0)
        self.slider_layout.addWidget(self.motor_speed_slider, 0, 1)
        self.slider_layout.addWidget(self.motor_speed_value, 0, 2)

        self.slider_layout.addWidget(self.load_speed_label, 1, 0)
        self.slider_layout.addWidget(self.load_speed_slider, 1, 1)
        self.slider_layout.addWidget(self.load_speed_value, 1, 2)

        self.main_layout = QVBoxLayout()
        self.main_layout.addLayout(self.slider_layout)

        self.setLayout(self.main_layout)

        # Connections.
        self.motor_speed_slider.valueChanged.connect(
            lambda val: self.motor_speed_value.setText(str(val)))
        self.load_speed_slider.valueChanged.connect(
            lambda val: self.load_speed_value.setText(str(val)))

        self.set_config(config)

    def get_config(self):
        config = {}
        config["motor_pwm"] = self.motor_speed_slider.value()
        config["load_pwm"] = self.load_speed_slider.value()
        return config

    def set_config(self, config):
        if config["motor_pwm"] > 255 or config["motor_pwm"] < -255:
            raise ValueError("Motor PWM must be between -255 and 255")
        self.motor_speed_slider.setValue(config["motor_pwm"])

        if config["load_pwm"] > 255 or config["load_pwm"] < -255:
            raise ValueError("Load PWM must be between -255 and 255")
        self.load_speed_slider.setValue(config["load_pwm"])
