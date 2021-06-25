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
pl.subplot(411);
pl.plot(data[a:b,0],data[a:b,1],"k");
pl.ylabel('R');
#
pl.subplot(412);
pl.plot(data[a:b,0],data[a:b,2],"g-");
pl.plot(data[a:b,0],data[a:b,3],"c-.");
pl.ylabel('IG-IB');
#
pl.subplot(413);
pl.plot(data[a:b,0],data[a:b,5],"g-");
pl.plot(data[a:b,0],data[a:b,6],"c-.");
pl.ylabel('w OFC G/B');
#
pl.subplot(414);
pl.plot(data[a:b,0],data[a:b,4],"k");
pl.ylabel('OFC');
#
#
pl.show();
