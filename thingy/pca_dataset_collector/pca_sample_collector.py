from matplotlib import pyplot as plt
import serial
import csv
import math
import numpy as np

# Make sure these params have the same values in sensors.h.

num_sensors = 1
num_axis_per_sensor = 3
total_samples_per_accel_channel = 256
delay_between_samples_us = 400

# Script start.
num_readings = num_sensors * num_axis_per_sensor
sampling_freq = 1 / (delay_between_samples_us / 1000000)
num_magnitudes = total_samples_per_accel_channel / 2 + 1
freq_start = 0
freq_step = sampling_freq / total_samples_per_accel_channel

freqs = [str(i * freq_step) for i in range(0, int(num_magnitudes))]
readings = [[0] * int(num_magnitudes)] * num_readings
readings_parsed = [[0] * int(num_magnitudes)] * num_readings
readings_average = [[0] * int(num_magnitudes)] * num_readings
max_mags = [0] * (num_sensors * num_axis_per_sensor)
num_reads = 0

def readings_to_lists(readings):
    res = [0] * len(readings)
    for i in range(len(readings)):
        try:
            res[i] = readings[i].decode("utf-8").strip()
            res[i] = [float(val) for val in res[i].split(",")]
            res[i] = [val if math.isfinite(val) else 0 for val in res[i]]
        except:
            return None

        if len(res[i]) != num_magnitudes:
            return None

    return res


def plot_single(ax, bar, reading, max_val):
    #bar.set_ydata(reading)
    for b, val in zip(bar, reading):
        b.set_height(val)
    #ax.set_ylim([min(reading), max(reading)])
    ax.set_ylim([0, max_val])

    ax.set_xlabel("Frequency [Hz]")
    ax.set_ylabel("Magnitude [m/s]")

ser = serial.Serial()
ser.baudrate = 921600
ser.port = "COM13"
ser.open()

fig, axs = plt.subplots(num_sensors, num_axis_per_sensor, squeeze=False)
bars = [0] * num_sensors * num_axis_per_sensor
i = 0

for col in range(num_axis_per_sensor):
    for row in range(num_sensors):
        #bars[i], = axs[row, col].plot(freqs, [0] * len(freqs))
        bars[i] = axs[row, col].bar(freqs, [0] * len(freqs))
        i += 1

        for label in axs[row][col].xaxis.get_ticklabels()[1:-1]:
            label.set_visible(False)

while True:
    ser.flush()

    # Wait for start of measurement.
    start_line = ""
    while start_line != "---":
        start_line = ser.readline()
        print(start_line)
        try:
            start_line = start_line.decode("utf-8").strip()
        except UnicodeDecodeError:
            ser.flush()
            continue

    # Read measurement.
    for i in range(num_readings):
        readings[i] = ser.readline()

    # Parse bytecode into lists.
    readings_parsed = readings_to_lists(readings)
    if not readings_parsed:
        continue

    #readings_average = np.array(readings_average) + 0.66 * (np.array(readings_parsed) - np.array(readings_average))

    readings_flattened = []
    for reading in readings_parsed:
        readings_flattened += reading

    with open("samples.csv", "a", newline='') as samples:
        writer = csv.writer(samples)
        writer.writerow(readings_flattened)

    # Display.
    i = 0
    for col in range(num_axis_per_sensor):
        for row in range(num_sensors):
            max_mags[i] = max(readings_parsed[i])
            plot_single(axs[row][col], bars[i], readings_parsed[i], max_mags[i])
            i += 1

    plt.pause(0.01)

