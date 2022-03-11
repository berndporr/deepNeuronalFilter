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
        self.alpha = 0.05
        print("Starting:")
        print(noisefolder,filtered_filename)
        self.snrdiff,self.av,self.sd = snr.calcAllSNRimprovemements(startsec = startsec,
                                                                    noisefolder = noisefolder,
                                                                    fs = fs,
                                                                    filtered_filename = filtered_filename)
        print(noisefolder,filtered_filename,"avg = {}, sd = {}".format(self.av,self.sd))
        t_value,p_value=stats.ttest_1samp(self.snrdiff,0)
        print(filtered_filename,'Test statistic is %f'%float("{:.6f}".format(t_value)))
        print('p-value for t-test is %f'%p_value)
        if p_value<=self.alpha:
            print(filtered_filename," significantly different from baseline.")


# check if we run this as a main program
if __name__ == "__main__":
    startsec = 60
    filtered_folder = "jawclench"
    helptext = 'usage: {} -s startsec -f file'.format(sys.argv[0])

    try:
        # Gather the arguments
        all_args = sys.argv[1:]
        opts, arg = getopt.getopt(all_args, 's:t:h')
        # Iterate over the options and values
        for opt, arg_val in opts:
            if '-s' in opt:
                startsec = int(arg_val)
            elif '-t' in opt:
                filtered_folder = arg_val
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        sys.exit(2)


        
#plotting the box plot of the data
dnfsnr = SNRStat(500,filtered_folder,startsec,"dnf.tsv")
lmssnr = SNRStat(500,filtered_folder,startsec,"lms.tsv")
box_fig = plt.figure('compare')
plt.bar(["DNF","LMS"],
        height = [dnfsnr.av,lmssnr.av],
        yerr = [dnfsnr.sd,lmssnr.sd],
        align='center', capsize=5)
plt.scatter(np.zeros(len(dnfsnr.snrdiff)),dnfsnr.snrdiff)
plt.scatter(np.ones(len(dnfsnr.snrdiff)),lmssnr.snrdiff)
box_fig.savefig('./SNRcompareDNFLMS.eps',
                format='eps', bbox_inches='tight')


t_value,p_value=stats.ttest_rel(dnfsnr.snrdiff,lmssnr.snrdiff)
print('DNF vs LMS test statistic is %f'%float("{:.6f}".format(t_value)))
print('p-value for the t-test is %f'%p_value)
if p_value<=dnfsnr.alpha:
    print("Significantly different between DNF and LMS.")


plt.show()
