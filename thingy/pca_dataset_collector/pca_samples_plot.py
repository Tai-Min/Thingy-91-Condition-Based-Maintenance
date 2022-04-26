from matplotlib import pyplot as plt
import pandas as pd

df = pd.read_csv("samples.csv")
samples = df.to_numpy()

for samples_col1 in range(samples.shape[1]):
    for samples_col2 in range(samples_col1 + 1, samples.shape[1]):
        plt.scatter(samples[:, samples_col1], samples[:, samples_col2])
        plt.title("Columns %d and %d" % (samples_col1, samples_col2))
        plt.pause(0.1)
        plt.clf()