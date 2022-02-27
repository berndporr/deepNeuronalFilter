#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed May 13 20:09:18 2020

@author: Sama
"""

import numpy as np
import matplotlib.pylab as plt
import statistics as stat
from scipy import stats

plt.close("all")

snrdiff = np.loadtxt("inner2dnf.dat")

t_value,p_value=stats.ttest_1samp(snrdiff,0)

print('Test statistic is %f'%float("{:.6f}".format(t_value)))

print('p-value for two tailed test is %f'%p_value)

alpha = 0.05

if p_value<=alpha:
    print("Significantly different.")

# calculating the average
lenData = 12.00
snrdiff_av = np.sum(snrdiff)/lenData
print("Avg=",snrdiff_av)

# calculating the standard deviation
snrdnf_sd = stat.stdev(snrdiff)
print("SD=",snrdnf_sd)

#plotting the box plot of the data
box_fig = plt.figure('compare')
plt.bar([0], height = snrdiff_av, yerr = snrdiff_av, align='center', capsize=5)
box_fig.savefig('./SNRs_errorbars_compare',
                format='eps', bbox_inches='tight')
plt.show()










