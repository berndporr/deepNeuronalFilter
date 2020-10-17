#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Apr 26 19:51:56 2020

@author: Sama
"""
import scipy.signal as sig
import numpy as np
import matplotlib.pylab as plt

# Sampling rate
fs = 250
cf = np.arange(0, 126, 1) / fs * 2

outer = sig.firwin(249, [cf[2], cf[45], cf[55]], window='hamming', pass_zero=False)
np.savetxt('../forOuter.dat', outer)
fig1 = plt.figure(1)
ax1 = fig1.add_subplot(111)
plot1 = ax1.plot(outer)
plt.show()

inner = sig.firwin(249, [cf[2], cf[45], cf[55]], window='hamming', pass_zero=False)
np.savetxt('../forInner.dat', inner)
fig2 = plt.figure(2)
ax2 = fig2.add_subplot(111)
plot2 = ax2.plot(inner[20:30])
plt.show()

