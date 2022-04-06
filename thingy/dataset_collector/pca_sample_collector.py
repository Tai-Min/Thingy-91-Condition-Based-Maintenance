from matplotlib import pyplot as plt
import serial
import csv

# Make sure these params have the same values in sensors.h.

num_sensors = 2
num_axis_per_sensor = 3
total_samples_per_accel_channel = 16
delay_between_samples_us = 200

# Script start.

num_readings = num_sensors * num_axis_per_sensor
sampling_freq = 1 / (delay_between_samples_us / 1000000)
num_magnitudes = total_samples_per_accel_channel / 2 + 1
freq_start = 0
freq_step = sampling_freq / total_samples_per_accel_channel

freqs = [str(i * freq_step) for i in range(int(num_magnitudes))]
readings = [0] * num_readings
max_mags = [0] * num_readings


def readings_to_lists(readings):
    res = [0] * len(readings)
    for i in range(len(readings)):
        try:
            res[i] = readings[i].decode("utf-8").strip()
            res[i] = [float(val) for val in res[i].split(",")]
        except:
            return None

        if len(res[i]) != num_magnitudes:
            return None

    return res


def plot_single(reading, upper_limit, row, col):
    axs[row, col].cla()
    axs[row, col].bar(freqs, reading)
    axs[row, col].set_ylim([0, upper_limit])
    for label in axs[row, col].xaxis.get_ticklabels()[1:-1: 2]:
        label.set_visible(False)


ser = serial.Serial()
ser.baudrate = 115200
ser.port = "COM13"
ser.open()

fig, axs = plt.subplots(num_sensors, num_axis_per_sensor)

while True:
    # Wait for start of measurement.
    start_line = ""
    while start_line != "---":
        
        start_line = ser.readline()
        print(start_line)

        try:
            start_line = start_line.decode("utf-8").strip()
        except UnicodeDecodeError:
            continue

    # Read measurement.
    for i in range(num_readings):
        readings[i] = ser.readline()

    # Parse bytecode into lists.
    readings_parsed = readings_to_lists(readings)
    if not readings_parsed:
        continue

    readings_flattened = []
    for reading in readings_parsed:
        readings_flattened += reading

    with open("samples.csv", "a", newline='') as samples:
        writer = csv.writer(samples)
        writer.writerow(readings_flattened)

    # Display.
    i = 0
    for row in range(num_sensors):
        for col in range(num_axis_per_sensor):
            max_mags[i] = max(max_mags[i], max(readings_parsed[i]))

            plot_single(readings_parsed[i], max_mags[i], row, col)
            i += 1

    plt.pause(0.05)
