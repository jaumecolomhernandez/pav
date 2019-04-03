#!/bin/python
import matplotlib.pyplot as plt
import sys
import numpy as np

num='03'

for i in range(10,50):
    data = np.loadtxt('./exports/rl0'+str(i)+'.f0')
    data2 = np.loadtxt('./exports/rl0'+str(i)+'.f0ref')
    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1)
    ax.plot(data)
    ax.plot(data2)

data = np.loadtxt('./exports/rl0'+num+'.f0')
data2 = np.loadtxt('./exports/rl0'+num+'.f0ref')
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.plot(data)
ax.plot(data2)
plt.show()
