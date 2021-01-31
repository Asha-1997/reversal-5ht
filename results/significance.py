#!/usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as signal
import scipy.stats as stats
import math as math

exp = ['normal','drn_suppress','drn_suppress_ssri','drn_suppress_5ht2up']


average = []
stddev = []

def doTest(test_for):
    for e1 in exp:
        perf1=np.loadtxt(e1+"/perf.dat")
        nreward1 = perf1[:,1] / perf1[:,2]
        for e2 in exp:
            if (not e1 == e2):
                perf2=np.loadtxt(e2+"/perf.dat")
                nreward2 = perf2[:,1] / perf2[:,2]
                m = min((len(nreward1),len(nreward2)))
                t, p = stats.ttest_rel(nreward1[:m], nreward2[:m])
                significant = (p < 0.05)
                if significant == test_for:
                    print(e1,e2,p,significant,sep="\t")


print("Significantly different:")
doTest(True)

print("\n\nNo significance:")
doTest(False)
