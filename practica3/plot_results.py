#!/bin/python
import matplotlib.pyplot as plt
import sys
import numpy as np
base_path = '/home/jc/pav/practica3/'
num='48'
for i in range(10,50):
    data = np.loadtxt(base_path+'exports/sb0'+str(i)+'.f0')
    data2 = np.loadtxt(base_path+'exports/sb0'+str(i)+'.f0ref')
    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1)
    ax.plot(data)
    ax.plot(data2)
plt.show()