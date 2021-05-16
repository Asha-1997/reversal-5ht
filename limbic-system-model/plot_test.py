#!/usr/bin/python3

import numpy as np
import pylab as pl
#
data = np.loadtxt('ctxneuron_test.dat');
#
fig = pl.figure(1)
#
a = 0
b = len(data[:,0])
# compound
pl.subplot(311);
pl.plot(data[a:b,0],data[a:b,1]);
pl.plot(data[a:b,0],data[a:b,2]);
pl.plot(data[a:b,0],data[a:b,3]);
pl.ylabel('R-IG-IB');
#
pl.subplot(312);
pl.plot(data[a:b,0],data[a:b,5]);
pl.plot(data[a:b,0],data[a:b,6]);
pl.ylabel('w OFC G/B');
#
pl.subplot(313);
pl.plot(data[a:b,0],data[a:b,4]);
pl.ylabel('OFC');
#
#
pl.show();
