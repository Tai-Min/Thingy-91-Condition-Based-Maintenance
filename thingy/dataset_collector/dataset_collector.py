from parallelplot import parallel_plot
from matplotlib import pyplot as plt
import serial

# Make sure these params have the same values in sensors.h.

num_sensors = 2
num_axis_per_sensor = 3
total_samples_per_accel_channel = 16
delay_between_samples_us = 1000

# Script start.

num_readings = num_sensors * num_axis_per_sensor
sampling_freq = 1 / (delay_between_samples_us / 1000000)
num_magnitudes = total_samples_per_accel_channel / 2 + 1
freq_start = 0
freq_step = sampling_freq / total_samples_per_accel_channel

freqs = [str(i * freq_step) for i in range(int(num_magnitudes))]
readings = [0] * num_readings


def plot_single(reading, row, col):
    if type(reading) != bytes:
        return

    data = reading.decode("utf-8").strip()
    data = [float(val) for val in data.split(",")]

    if len(data) != num_magnitudes:
        return

    axs[row, col].cla()
    axs[row, col].set_ylim([0, 300])
    axs[row, col].bar(freqs, data)


ser = serial.Serial()
ser.baudrate = 115200
ser.port = "COM13"
ser.open()

fig, axs = plt.subplots(num_sensors, num_axis_per_sensor)

while True:
    try:
        start_line = ""
        while start_line != "---":
            start_line = ser.readline()
            start_line = start_line.decode("utf-8").strip()

        for i in range(num_readings):
            readings[i] = ser.readline()

        i = 0
        for row in range(num_sensors):
            for col in range(num_axis_per_sensor):
                plot_single(readings[i], row, col)
                i += 1

        plt.pause(0.05)

    except UnicodeDecodeError:
        continue
