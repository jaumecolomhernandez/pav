#!/bin/python

import matplotlib.pyplot as plt
import numpy as np

for file_name in ["power", "amplitude", "zero_cross_rate"]:
    #Llegim el fitxer i el transformem a una array de numpy
    arr = np.fromfile(f"./exports/{file_name}", dtype="float", sep="\n")
    #print(arr.shape)
    #print(arr)
    
    #Creem el plot i el guardem
    fig, ax = plt.subplots()
    ax.plot(arr)
    ax.set(xlabel='Sample', ylabel='Value',
        title=f'{file_name.capitalize()} plot')
    ax.grid()
    fig.savefig(f"./images/{file_name}.png")
    plt.show()