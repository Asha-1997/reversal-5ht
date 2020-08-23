#!/usr/bin/python3

import numpy as np
import pylab as pl
#
data = np.loadtxt('log.dat');
#
fig = pl.figure(1)
#
a = 0
b = len(data[:,0])
# compound
pl.subplot(711);
pl.plot(data[a:b,0],data[a:b,6]);
pl.plot(data[a:b,0],data[a:b,2]);
pl.ylabel('vis1/green lm');
pl.ylim([0,1.2])
pl.yticks([0,0.5,1])
#
pl.subplot(712);
pl.plot(data[a:b,0],data[a:b,28]);
pl.ylabel('OFC');
pl.ylim([0,1])
pl.yticks([0,0.5,1])
#
pl.subplot(713);
pl.plot(data[a:b,0],data[a:b,9]);
pl.ylabel('mPFC: green lm');
pl.ylim([0,2.5])
pl.yticks([0,1,2])
#
pl.subplot(714);
pl.plot(data[a:b,0],data[a:b,25]);
pl.ylabel('DRN');
pl.ylim([0,2.5])
pl.yticks([0,1,2])
#
pl.subplot(715);
pl.plot(data[a:b,0],data[a:b,8]);
pl.ylabel('core w green');
pl.ylim([0,1.2])
pl.yticks([0,0.5,1])
#
pl.subplot(716);
pl.plot(data[a:b,0],data[a:b,11]);
pl.ylabel('core w blue');
pl.ylim([0,3])
pl.yticks([0,1,2])
#
pl.subplot(717);
pl.plot(data[a:b,0],data[a:b,1]*2);
pl.plot(data[a:b,0],data[a:b,14]);
pl.ylabel('VTA');
pl.ylim([0,0.6])
pl.yticks([0,0.25,0.5])
#
#
pl.show();
