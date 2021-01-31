#!/usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as signal
import math as math

# exp = ['normal','normal_less_wait','drn_suppress','drn_suppress_less_wait','drn_suppress_ssri','drn_suppress_ssri_less_wait','drn_suppress_5ht2up','drn_suppress_5ht2up_less_wait']

exp = ['normal','drn_suppress','drn_suppress_ssri']

average = []
stddev = []

for e in exp:
    perf=np.loadtxt(e+"/perf.dat")
    nreward = perf[:,1] / perf[:,2]
    average.append(np.mean(nreward))
    stddev.append(np.std(nreward))

index = np.arange(len(exp))
height = 0.35
fig, ax = plt.subplots()
rects = ax.barh(index+height*1.1,average,height,align='edge',xerr=stddev)
ax.set_xlabel('r')
ax.set_title('Relative reward')
ax.set_yticks(index + height)
ax.set_yticklabels(exp)
plt.show()
