from tracemalloc import start
import serial
import csv
from os.path import exists

# Make sure these params have the same values in pca.h.

label = "struggle"
pca_result_size = 65


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

find_available_filename.cntr = 1

ser = serial.Serial()
ser.baudrate = 921600
ser.port = "COM13"
ser.open()

while True:
    ser.flush()

    # Wait for start of measurement.
    start_line = ""
    while start_line != "---":

        start_line = ser.readline()

        try:
            start_line = start_line.decode("utf-8").strip()
        except UnicodeDecodeError:
            continue

    # Read measurement.
    reading = ser.readline()

    # Parse bytecode into lists.
    reading_parsed = reading_to_lists(reading)
    if not reading_parsed:
        continue

    with open(find_available_filename(), "a", newline='') as file:
        writer = csv.writer(file)
        writer.writerow(csv_header)
        writer.writerow(reading_parsed)
