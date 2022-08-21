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
    def __init__(self,fs,startsec,filtered_filename):
        self.alpha = 0.05
        print("Starting:")
        print(filtered_filename)
        self.before,self.after,self.av,self.sd = snr.calcAllSNRimprovemements(startsec = startsec,
                                                                              fs = fs,
                                                                              filtered_filename = filtered_filename)
        self.snrdiff = self.after - self.before
        print(filtered_filename,"avg = {}, sd = {}".format(self.av,self.sd))
        t_value,p_value=stats.ttest_1samp(self.snrdiff,0)
        print(filtered_filename,'Test statistic is %f'%float("{:.6f}".format(t_value)))
        print('p-value for t-test is %f'%p_value)
        if p_value<=self.alpha:
            print(filtered_filename," significantly different from baseline.")


# check if we run this as a main program
if __name__ == "__main__":
    startsec = 60
    helptext = 'usage: {} -s startsec -f file'.format(sys.argv[0])

    try:
        # Gather the arguments
        all_args = sys.argv[1:]
        opts, arg = getopt.getopt(all_args, 's:t:h')
        # Iterate over the options and values
        for opt, arg_val in opts:
            if '-s' in opt:
                startsec = int(arg_val)
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        sys.exit(2)



#plotting the box plot of the data
dnfsnr = SNRStat(500,startsec,"dnf.tsv")
lmssnr = SNRStat(500,startsec,"lms.tsv")
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

dnf_impr = plt.figure("DNF improvements")
for i in range(len(dnfsnr.snrdiff)):
    y = np.array([dnfsnr.before[i],dnfsnr.after[i]])
    x = np.array([0,1])
    plt.scatter(0,dnfsnr.before[i])
    plt.scatter(1,dnfsnr.after[i])
    plt.ylim([-12.5,2.5])
    plt.plot(x,y)

dnf_impr.savefig('./SNR_DNF_improvements.eps',
                 format='eps', bbox_inches='tight')


lms_impr = plt.figure("LMS improvements")
for i in range(len(lmssnr.snrdiff)):
    y = np.array([lmssnr.before[i],lmssnr.after[i]])
    x = np.array([0,1])
    plt.scatter(0,lmssnr.before[i])
    plt.scatter(1,lmssnr.after[i])
    plt.ylim([-12.5,2.5])
    plt.plot(x,y)

lms_impr.savefig('./SNR_LMS_improvements.eps',
                 format='eps', bbox_inches='tight')

plt.show()
