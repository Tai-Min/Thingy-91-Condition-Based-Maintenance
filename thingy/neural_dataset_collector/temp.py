import pandas as pd
import os





files = os.listdir("./dataset")

for file in files:
    df = pd.read_csv("./dataset/%s" % file)

    tstamp = 0
    stamps = [i * 50 for i in range(0, len(df["timestamp"]))]

    df["timestamp"] = stamps
    df.to_csv("./dataset/%s" % file, index=False)