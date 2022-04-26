from PyQt5.QtWidgets import *

from pymodbus.client.sync import ModbusSerialClient
from ctypes import c_uint16
import time
import os
import yaml

from connection_widget import ConnectionWidget
from motor_control_widget import MotorControlWidget
from bearing_control_widget import BearingControlWidget
from noise_control_widget import NoiseControlWidget
from impact_control_widget import ImpactControlWidget

# Coil addresses.
IMPACT_MOTOR_ENABLE_FIXED_ADDR = 0
IMPACT_MOTOR_ENABLE_RANDOM_ADDR = 1
IMPACT_LOAD_ENABLE_FIXED_ADDR = 2
IMPACT_LOAD_ENABLE_RANDOM_ADDR = 3
BEARING_ENABLE_ADDR = 4
NOISE_ENABLE_ADDR = 5

# Holding register addresses.
MOTOR_SPEED_ADDR = 0
LOAD_SPEED_ADDR = 1

MOTOR_SHOCK_PERIOD_ADDR = 2
MOTOR_SHOCK_FILLING_ADDR = 3
LOAD_SHOCK_PERIOD_ADDR = 4
LOAD_SHOCK_FILLING_ADDR = 5

BEARING_PULSE_DURATION_ADDR = 6
BEARING_PULSE_THRESHOLD_ADDR = 7
BEARING_PULSES_PER_REV_ADDR = 8


class MainWindow(QMainWindow):
    def __init__(self, config, script_path, current_scenario, parent=None):
        super(MainWindow, self).__init__(parent)

        self.script_path = script_path
        self.set_current_scenario(current_scenario)

        self.main_window_widget = QWidget()
        self.setCentralWidget(self.main_window_widget)

        # GUI components.

        self.menu_bar = QMenuBar(self)
        self.file_menu = QMenu("File", self)
        self.open_action = QAction("Open...", self)
        self.open_action.setShortcut("Ctrl+O")
        self.save_action = QAction("Save", self)
        self.save_action.setShortcut("Ctrl+S")
        self.save_as_action = QAction("Save as...", self)
        self.save_as_action.setShortcut("Ctrl+Shift+S")
        self.file_menu.addAction(self.open_action)
        self.file_menu.addSeparator()
        self.file_menu.addAction(self.save_action)
        self.file_menu.addAction(self.save_as_action)
        self.menu_bar.addMenu(self.file_menu)
        self.setMenuBar(self.menu_bar)

        self.connection_widget = ConnectionWidget()
        self.motor_widget = MotorControlWidget(config["motor_control"])
        self.impact_widget = ImpactControlWidget(config["impact_control"])
        self.bearing_widget = BearingControlWidget(config["bearing_control"])
        self.noise_widget = NoiseControlWidget(config["noise_control"])

        self.write_button = QPushButton("WRITE")
        self.write_button.setEnabled(False)
        self.write_button.setFixedWidth(70)

        self.stop_button = QPushButton("STOP")
        self.stop_button.setEnabled(False)
        self.stop_button.setFixedWidth(70)

        # Layouts.
        self.left_layout = QVBoxLayout()
        self.left_layout.addWidget(self.motor_widget)
        self.left_layout.addWidget(self.impact_widget)

        self.right_layout = QVBoxLayout()
        self.right_layout.addWidget(self.bearing_widget)
        self.right_layout.addWidget(self.noise_widget)

        self.controls_layout = QHBoxLayout()
        self.controls_layout.addLayout(self.left_layout)
        self.controls_layout.addLayout(self.right_layout)

        self.server_layout = QHBoxLayout()
        self.server_layout.addStretch()
        self.server_layout.addWidget(self.write_button)
        self.server_layout.addWidget(self.stop_button)
        self.server_layout.addStretch()

        self.main_layout = QVBoxLayout()
        self.main_layout.addWidget(self.connection_widget)
        self.main_layout.addLayout(self.controls_layout)
        self.main_layout.addLayout(self.server_layout)

        self.main_window_widget.setLayout(self.main_layout)

        # Connections.
        self.connection_widget.connect_button.pressed.connect(
            self.connect_to_slave)

        self.write_button.pressed.connect(self.write_server)
        self.stop_button.pressed.connect(self.stop_actuators)

        self.open_action.triggered.connect(self.open_scenario)
        self.save_action.triggered.connect(self.save_scenario)
        self.save_as_action.triggered.connect(self.save_scenario_as)

    def open_scenario(self):
        file = QFileDialog.getOpenFileName(
            self, "Open scenario", ".", "Config files (*.yaml)")
        file = file[0]

        if not file:
            return

        with open(file, "r") as stream:
            config = yaml.safe_load(stream)

            self.motor_widget.set_config(config["motor_control"])
            self.impact_widget.set_config(config["impact_control"])
            self.bearing_widget.set_config(config["bearing_control"])
            self.noise_widget.set_config(config["noise_control"])

            self.set_current_scenario(file)

    def save_scenario(self):
        if os.path.normpath(self.current_scenario) == os.path.normpath(os.path.join(self.script_path, "default.yaml")):
            self.save_scenario_as()
        else:
            self.save_config_to_file(self.current_scenario)

    def save_scenario_as(self):
        ok = False

        while not ok:
            file = QFileDialog.getSaveFileName(
                self, "Open scenario", ".", "Config files (*.yaml)")
            file = file[0]

            if not file:
                return
            elif os.path.normpath(file) == os.path.normpath(os.path.join(self.script_path, "default.yaml")):
                msg = QMessageBox()
                msg.setIcon(QMessageBox.Critical)
                msg.setWindowTitle("Couldn't save the file")
                msg.setText("Default scenario is protected from saving.")
                msg.exec_()
            else:
                ok = True

        self.save_config_to_file(file)
        self.set_current_scenario(file)

    def set_current_scenario(self, path):
        with open("state", "w") as stream:
            stream.write(path)

        self.current_scenario = path
        self.setWindowTitle("Workbench [%s]" % path)

    def save_config_to_file(self, file):
        config = {}
        config["motor_control"] = self.motor_widget.get_config()
        config["impact_control"] = self.impact_widget.get_config()
        config["bearing_control"] = self.bearing_widget.get_config()
        config["noise_control"] = self.noise_widget.get_config()

        with open(file, "w") as stream:
            yaml.dump(config, stream)

    def connect_to_slave(self):
        self.unit_addr = int(self.connection_widget.unit_widget.text())

        self.modbus_client = ModbusSerialClient(method='rtu', port=self.connection_widget.port_widget.text(
        ), baudrate=int(self.connection_widget.baud_widget.text()))
        if(self.modbus_client.connect()):
            time.sleep(2)
            self.write_button.setEnabled(True)
            self.stop_button.setEnabled(True)
            self.connection_widget.connect_button.setEnabled(False)

        else:
            msg = QMessageBox()
            msg.setIcon(QMessageBox.Critical)
            msg.setWindowTitle("Connection error")
            msg.setText("Couldn't connect to slave.")
            msg.exec_()

    def write_server(self):
        # Motor control.
        self.modbus_client.write_register(unit=self.unit_addr, address=MOTOR_SPEED_ADDR, value=c_uint16(
            self.motor_widget.motor_speed_slider.value()).value)
        self.modbus_client.write_register(unit=self.unit_addr, address=LOAD_SPEED_ADDR, value=c_uint16(
            self.motor_widget.load_speed_slider.value()).value)

        # Impact control.
        self.modbus_client.write_coil(
            unit=self.unit_addr, address=IMPACT_MOTOR_ENABLE_FIXED_ADDR, value=self.impact_widget.motor_enable_fixed_button.isChecked())
        self.modbus_client.write_coil(
            unit=self.unit_addr, address=IMPACT_MOTOR_ENABLE_RANDOM_ADDR, value=self.impact_widget.motor_enable_random_button.isChecked())
        self.modbus_client.write_register(unit=self.unit_addr, address=MOTOR_SHOCK_PERIOD_ADDR, value=c_uint16(
            self.impact_widget.motor_shock_period_slider.value()).value)
        self.modbus_client.write_register(unit=self.unit_addr, address=MOTOR_SHOCK_FILLING_ADDR, value=c_uint16(
            self.impact_widget.motor_shock_filling_slider.value()).value)

        self.modbus_client.write_coil(
            unit=self.unit_addr, address=IMPACT_LOAD_ENABLE_FIXED_ADDR, value=self.impact_widget.load_enable_fixed_button.isChecked())
        self.modbus_client.write_coil(
            unit=self.unit_addr, address=IMPACT_LOAD_ENABLE_RANDOM_ADDR, value=self.impact_widget.load_enable_random_button.isChecked())
        self.modbus_client.write_register(unit=self.unit_addr, address=LOAD_SHOCK_PERIOD_ADDR, value=c_uint16(
            self.impact_widget.load_shock_period_slider.value()).value)
        self.modbus_client.write_register(unit=self.unit_addr, address=LOAD_SHOCK_FILLING_ADDR, value=c_uint16(
            self.impact_widget.load_shock_filling_slider.value()).value)

        # Bearing control
        self.modbus_client.write_coil(
            unit=self.unit_addr, address=BEARING_ENABLE_ADDR, value=self.bearing_widget.bearing_enable_button.isChecked())
        self.modbus_client.write_register(unit=self.unit_addr, address=BEARING_PULSE_DURATION_ADDR, value=c_uint16(
            self.bearing_widget.servo_pulse_duration_slider.value()).value)
        self.modbus_client.write_register(unit=self.unit_addr, address=BEARING_PULSE_THRESHOLD_ADDR, value=c_uint16(
            self.bearing_widget.servo_pulse_threshold_slider.value()).value)
        self.modbus_client.write_register(unit=self.unit_addr, address=BEARING_PULSES_PER_REV_ADDR, value=c_uint16(
            self.bearing_widget.servo_pulse_count_slider.value()).value)

        # Noise control.
        self.modbus_client.write_coil(unit=self.unit_addr, address=NOISE_ENABLE_ADDR, value=self.noise_widget.noise_enable_button.isChecked())

    def stop_actuators(self):
        self.motor_widget.motor_speed_slider.setValue(0)
        self.motor_widget.load_speed_slider.setValue(0)
        self.noise_widget.noise_enable_button.setChecked(False)
        self.write_server()
