#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed May 13 20:09:18 2020

@author: Sama
"""

import numpy as np
import matplotlib.pylab as plt
import statistics as stat

plt.close("all")

# below are the SNR ratios (i.e. SNR after filtering divided by the SNR before filtering)

delta_DFN = np.array([ 2.87182448,  1.7200778 ,  1.1209422 ,  5.07132132,  6.66344464,
       42.03250341,  3.58688525,  2.57380138,  2.26092613,  1.1178061 ,
       58.44328626,  4.29061414])

alpha_DFN = np.array([  1.8489426 ,   1.35840708, 172.        ,   5.20967742,
         7.73076923,   1.24271229,  15.83333333, 113.        ,
        31.5       ,   1.07537355,   3.66666667,   1.36979691])
    
delta_LMS = np.array([5.99558621, 1.00007307, 1.13637761, 1.44002081, 1.14656466,
       2.40944311, 1.05131363, 1.00207624, 0.999863 , 1.00010127, 1.01518454,
       1.00103286])
    
alpha_LMS = np.array([1.73426573, 1.0001857 , 3.96      , 1.45321637, 1.07828283,
       1.00016566, 1.14171512, 1.        , 0.999998, 1.00793651, 1.        ,
       1.06184799])

delta_LPLC = np.array([1.84318417, 4.86232367, 1.00240641,
       1.13695574, 0.92221996, 1.64749088, 3.25269912,
       2.38510865, 6.74070778, 1.21574596, 0.6759837 , 4.42329092])

alpha_LPLC = np.array([0.75085805,  1.89036032,  1.64463669,
        1.36844885,  1.12423476,  0.41907288, 0.99009355,
        0.84337219,  0.77715078,  3.35616142,  3.56543171,  1.88493668])

# calculating the standard deviation
alpha_DFN_sd = stat.stdev(alpha_DFN)
delta_DFN_sd = stat.stdev(delta_DFN)
alpha_LMS_sd = stat.stdev(alpha_LMS)
delta_LMS_sd = stat.stdev(delta_LMS)
alpha_LPLC_sd = stat.stdev(alpha_LPLC)
delta_LPLC_sd = stat.stdev(delta_LPLC)

# calculating the average
lenData = 12.00
alpha_DFN_av = np.sum(alpha_DFN)/lenData
delta_DFN_av = np.sum(delta_DFN)/lenData
alpha_LMS_av = np.sum(alpha_LMS)/lenData
delta_LMS_av = np.sum(delta_LMS)/lenData
alpha_LPLC_av = np.sum(alpha_LPLC)/lenData
delta_LPLC_av = np.sum(delta_LPLC)/lenData

averages = [delta_DFN_av, alpha_DFN_av, delta_LMS_av, alpha_LMS_av, delta_LPLC_av, alpha_LPLC_av]
st_dev = [delta_DFN_sd, alpha_DFN_sd,  delta_LMS_sd, alpha_LMS_sd, delta_LPLC_sd, alpha_LPLC_sd]
    
#plotting the box plot of the data
box_fig = plt.figure('compare 3 filters')
plt.bar([0,1,3,4,6,7], averages, yerr = st_dev, bottom = 1, align='center', alpha=1, ecolor='black', capsize=5)
plt.yscale('log')
box_fig.savefig('./SNRs_errorbars_compare',
                quality=100, format='eps', bbox_inches='tight')
plt.show()










