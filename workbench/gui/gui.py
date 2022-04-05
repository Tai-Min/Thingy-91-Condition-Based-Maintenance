from PyQt5.QtWidgets import *
import sys
import yaml
import os
from main_window import MainWindow

if __name__ == '__main__':
    script_path = os.path.dirname(os.path.realpath(__file__))

    # Get path of last known scenario or fallback to default one.
    try:
        with open('state', 'r') as state:
            last_scenario = state.read()
    except:
        last_scenario = os.path.join(script_path, "default.yaml")

    # Open saved scenario.
    try:
        with open(last_scenario, 'r') as stream:
            config = yaml.safe_load(stream)
    except:
        last_scenario = os.path.join(script_path, "default.yaml")
        try:
            with open(last_scenario, 'r') as stream:
                config = yaml.safe_load(stream)
        except:
            raise FileNotFoundError("Couldn't load default.yaml")


    app = QApplication([])
    window = MainWindow(config, script_path, last_scenario)
    window.show()
    sys.exit(app.exec_())
