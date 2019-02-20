#!/bin/python

import matplotlib.pyplot as plt
import numpy as np

for file_name in ["power", "amplitude", "zero_cross_rate"]:
    arr = np.fromfile(f"./exports/{file_name}", dtype="float", sep="\n")
    #print(arr.shape)
    #print(arr)

    fig, ax = plt.subplots()
    ax.plot(arr)
    ax.set(xlabel='Sample', ylabel='Value',
        title=f'{file_name.capitalize()} plot')
    ax.grid()
    fig.savefig(f"./images/{file_name}.png")
    plt.show()