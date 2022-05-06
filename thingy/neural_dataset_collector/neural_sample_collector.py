from tracemalloc import start
import serial
import csv
from os.path import exists

# Make sure these params have the same values in pca.h.

label = "bearing"
readings_per_sample = 10
pca_result_size = 70
sample_window_size = 256
sample_window_shift = 128
num_new_samples = sample_window_size - sample_window_shift
single_reading_time = 0.0004
window_sample_time = single_reading_time * num_new_samples
window_sample_time = 50 # Just use 50ms and don't bother.

def reading_to_lists(reading):
    try:
        res = reading.decode("utf-8").strip()
        res = [float(val) for val in res.split(",")]
    except:
        return None

    if len(res) != pca_result_size:
        return None

    return res


def find_available_filename():
    fname = "./dataset/" + label + "." + str(find_available_filename.cntr) + ".csv"

    while exists(fname):
        find_available_filename.cntr += 1
        fname = "./dataset/" + label + "." + str(find_available_filename.cntr) + ".csv"

    return fname


csv_header = ["pca" + str(s) for s in range(pca_result_size)]
csv_header.insert(0, "timestamp")

find_available_filename.cntr = 1

ser = serial.Serial()
ser.baudrate = 921600
ser.port = "COM13"
ser.open()

readings = [[0] * (1 + pca_result_size)] * readings_per_sample

while True:
    sampling_success = True
    tstamp = 0

    for sample in range(readings_per_sample):
        ser.flush()

        # Wait for start of measurement.
        start_line = ""
        while start_line != "---":

            start_line = ser.readline()
            try:
                start_line = start_line.decode("utf-8").strip()
            except UnicodeDecodeError:
                sampling_success = False
                break

        if not sampling_success:
            break

        # Read measurement.
        reading = ser.readline()
        # Parse bytecode into lists.
        reading_parsed = reading_to_lists(reading)
        if not reading_parsed:
            sampling_success = False
            break

        reading_parsed.insert(0, tstamp)
        readings[sample] = reading_parsed
        
        tstamp += window_sample_time

    if sampling_success:
        with open(find_available_filename(), "a", newline='') as file:
            writer = csv.writer(file)
            writer.writerow(csv_header)
            for reading in readings:
                writer.writerow(reading)
