#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed May 13 20:09:18 2020

@author: Sama
"""

import numpy as np
import matplotlib.pylab as plt
from scipy import stats
import snr
import sys
import getopt


class SNRStat:
    def __init__(self,fs,noisefolder,startsec,filtered_filename):
        alpha = 0.05
        self.snrdiff,self.av,self.sd = snr.calcAllSNRimprovemements(startsec = startsec,
                                                                    noisefolder = noisefolder,
                                                                    fs = fs,
                                                                    filtered_filename = filtered_filename)
        print(noisefolder,filtered_filename,"avg = {}, sd = {}".format(self.av,self.sd))
        t_value,p_value=stats.ttest_1samp(self.snrdiff,0)
        print(filtered_filename,'Test statistic is %f'%float("{:.6f}".format(t_value)))
        print('p-value for two tailed test is %f'%p_value)
        if p_value<=alpha:
            print(filtered_filename," significantly different from baseline.")


# check if we run this as a main program
if __name__ == "__main__":
    startsec = 60
    filtered_filename = "dnf.tsv"

    helptext = 'usage: {} -s startsec -f file'.format(sys.argv[0])

    try:
        # Gather the arguments
        all_args = sys.argv[1:]
        opts, arg = getopt.getopt(all_args, 'p:s:f:n:')
        # Iterate over the options and values
        for opt, arg_val in opts:
            if '-s' in opt:
                startsec = int(arg_val)
            elif '-f' in opt:
                filtered_filename = arg_val
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        sys.exit(2)


        
#plotting the box plot of the data
jawssnr = SNRStat(500,"jawclench",startsec,filtered_filename)
readsnr = SNRStat(500,"read",startsec,filtered_filename)
p300snr = SNRStat(250,"results",startsec,filtered_filename)
box_fig = plt.figure('compare')
plt.bar([0,1,2],
        height = [jawssnr.av,readsnr.av,p300snr.av],
        yerr = [jawssnr.sd,readsnr.sd,p300snr.sd],
        align='center', capsize=5)
box_fig.savefig('./SNRcompareRestWithJawClench.eps',
                format='eps', bbox_inches='tight')
plt.show()










