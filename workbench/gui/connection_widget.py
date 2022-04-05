from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import *


class ConnectionWidget(QWidget):
    def __init__(self, parent=None):
        super(ConnectionWidget, self).__init__(parent=parent)

        # Components.
        self.port_label = QLabel("Port:")
        self.port_widget = QLineEdit()
        self.port_widget.setFixedWidth(125)

        self.baud_label = QLabel("Baud rate:")
        self.baud_widget = QLineEdit("115200")
        self.baud_widget.setFixedWidth(125)

        self.unit_label = QLabel("Slave address:")
        self.unit_widget = QLineEdit("1")
        self.unit_widget.setFixedWidth(75)

        self.connect_button = QPushButton("Connect")
        self.connect_button.setFixedWidth(75)

        # Layouts.
        self.main_layout = QGridLayout()
        self.main_layout.addWidget(self.port_label, 0, 0, Qt.AlignLeft)
        self.main_layout.addWidget(self.port_widget, 1, 0, Qt.AlignLeft)

        self.main_layout.addWidget(self.baud_label, 0, 1, Qt.AlignLeft)
        self.main_layout.addWidget(self.baud_widget, 1, 1, Qt.AlignLeft)

        self.main_layout.addWidget(self.unit_label, 0, 2, Qt.AlignLeft)
        self.main_layout.addWidget(self.unit_widget, 1, 2, Qt.AlignLeft)

        self.main_layout.addWidget(self.connect_button, 1, 3, Qt.AlignLeft)

        self.spacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)
        self.main_layout.addItem(self.spacer, 1, 3)

        self.setLayout(self.main_layout)

