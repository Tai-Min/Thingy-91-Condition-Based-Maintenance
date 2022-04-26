from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import *


class BearingControlWidget(QGroupBox):
    def __init__(self, config, parent=None):
        super(BearingControlWidget, self).__init__(
            "Bearing control", parent=parent)

        # Components.
        self.bearing_enable_button = QPushButton("Enable")
        self.bearing_enable_button.setCheckable(True)
        self.bearing_enable_button.setFixedWidth(70)

        self.servo_pulse_duration_label = QLabel("Pulse duration:")
        self.servo_pulse_duration_slider = QSlider(Qt.Horizontal)
        self.servo_pulse_duration_slider.setMinimum(0)
        self.servo_pulse_duration_slider.setMaximum(500)
        self.servo_pulse_duration_value = QLabel(
            str(config["pulse_duration"]) + " [ms]")
        self.servo_pulse_duration_value.setFixedWidth(40)

        self.servo_pulse_threshold_label = QLabel("Pulse threshold:")
        self.servo_pulse_threshold_slider = QSlider(Qt.Horizontal)
        self.servo_pulse_threshold_slider.setMinimum(0)
        self.servo_pulse_threshold_slider.setMaximum(1023)
        self.servo_pulse_threshold_value = QLabel(
            str(config["pulse_threshold"]))
        self.servo_pulse_threshold_value.setFixedWidth(40)

        self.servo_pulse_count_label = QLabel("Pulses per revolution:")
        self.servo_pulse_count_slider = QSlider(Qt.Horizontal)
        self.servo_pulse_count_slider.setMinimum(1)
        self.servo_pulse_count_slider.setMaximum(8)
        self.servo_pulse_count_value = QLabel(
            str(config["pulses_per_revolution"]))
        self.servo_pulse_count_value.setFixedWidth(40)

        # Layouts.
        self.slider_layout = QGridLayout()

        self.slider_layout.addWidget(self.servo_pulse_duration_label, 2, 0)
        self.slider_layout.addWidget(self.servo_pulse_duration_slider, 2, 1)
        self.slider_layout.addWidget(self.servo_pulse_duration_value, 2, 2)

        self.slider_layout.addWidget(self.servo_pulse_threshold_label, 3, 0)
        self.slider_layout.addWidget(self.servo_pulse_threshold_slider, 3, 1)
        self.slider_layout.addWidget(self.servo_pulse_threshold_value, 3, 2)

        self.slider_layout.addWidget(self.servo_pulse_count_label, 4, 0)
        self.slider_layout.addWidget(self.servo_pulse_count_slider, 4, 1)
        self.slider_layout.addWidget(self.servo_pulse_count_value, 4, 2)

        self.main_layout = QVBoxLayout()
        self.main_layout.addWidget(self.bearing_enable_button)
        self.main_layout.addLayout(self.slider_layout)
        self.main_layout.setAlignment(
            self.bearing_enable_button, Qt.AlignCenter)

        self.setLayout(self.main_layout)

        # Connections.
        self.bearing_enable_button.toggled.connect(self.toggle_controls)

        self.servo_pulse_duration_slider.valueChanged.connect(
            lambda val: self.servo_pulse_duration_value.setText(str(val) + " [ms]"))
        self.servo_pulse_threshold_slider.valueChanged.connect(
            lambda val: self.servo_pulse_threshold_value.setText(str(val)))
        self.servo_pulse_count_slider.valueChanged.connect(
            lambda val: self.servo_pulse_count_value.setText(str(val)))

        self.toggle_controls(False)
        self.set_config(config)

    def toggle_controls(self, enabled):
        self.servo_pulse_duration_slider.setEnabled(enabled)
        self.servo_pulse_threshold_slider.setEnabled(enabled)
        self.servo_pulse_count_slider.setEnabled(enabled)

    def get_config(self):
        config = {}
        config["enabled"] = self.bearing_enable_button.isChecked()
        config["pulse_duration"] = self.servo_pulse_duration_slider.value()
        config["pulse_threshold"] = self.servo_pulse_threshold_slider.value()
        config["pulses_per_revolution"] = self.servo_pulse_count_slider.value()
        return config

    def set_config(self, config):
        self.bearing_enable_button.setChecked(config["enabled"])

        if config["pulse_duration"] > 500 or config["pulse_duration"] < 0:
            raise ValueError("Pulse duration must be between 0 and 200 [ms]")
        self.servo_pulse_duration_slider.setValue(config["pulse_duration"])

        if config["pulse_threshold"] > 1023 or config["pulse_threshold"] < 0:
            raise ValueError("Pulse threshold must be between 0 and 1023")
        self.servo_pulse_threshold_slider.setValue(config["pulse_threshold"])

        if config["pulses_per_revolution"] > 8 or config["pulses_per_revolution"] < 0:
            raise ValueError("Pulses per revolution must be between 0 and 8")
        self.servo_pulse_count_slider.setValue(config["pulses_per_revolution"])