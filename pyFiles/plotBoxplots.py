#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed May 13 20:09:18 2020

@author: Sama
"""

import numpy as np
import matplotlib.pylab as plt
import seaborn as sns
import pandas as pd

myData = 2 # 0 for DFN, 1 for LMS, 2 for Laplace
fileName = ['_DNF', '_LMS', '_LPLC']

goodOnes = np.loadtxt('../cppData{}/py_goodOnes.txt'.format(fileName[myData]), int)
print('goodOnes: ', goodOnes)

N_alpha = len(goodOnes)
N_delta = len(goodOnes)


plt.close("all")

snr_alpha_12 = np.zeros(N_alpha)
snr_alpha = np.zeros(N_alpha)
snr_alpha_nn = np.zeros(N_alpha)
snr_alpha_ratio = np.zeros(N_alpha)
snr_delta_12 = np.zeros(N_delta)
snr_delta = np.zeros(N_delta)
snr_delta_nn = np.zeros(N_delta)
snr_delta_ratio = np.zeros(N_delta)

my_whisk = 1000

j = 0
for i in goodOnes:
    data_12 = np.loadtxt('../cppData_DNF/subject{}/py_subject_{}_SNRs.txt'.format( i, i))
    data = np.loadtxt('../cppData{}/subject{}/py_subject_{}_SNRs.txt'.format(fileName[myData], i, i))
    snr_alpha_12[j] = data_12[0]
    snr_alpha[j] = data[0] 
    correction_for_alpha = snr_alpha_12[j] / snr_alpha[j]
    snr_alpha[j] = snr_alpha[j] * correction_for_alpha
    snr_alpha_nn[j] = data[1] * correction_for_alpha
    snr_alpha_ratio[j] = snr_alpha_nn[j]/snr_alpha[j]
    j += 1

j = 0
for i in goodOnes:
    data_12 = np.loadtxt('../cppData_DNF/subject{}/py_subject_{}_SNRs.txt'.format( i, i))
    data = np.loadtxt('../cppData{}/subject{}/py_subject_{}_SNRs.txt'.format(fileName[myData], i, i))
    snr_delta_12[j] = data_12[3]
    snr_delta[j] = data[3]
    correction_for_delta = snr_delta_12[j] / snr_delta[j]
    snr_delta[j] = snr_delta[j] * correction_for_delta
    snr_delta_nn[j] = data[4] * correction_for_delta
    snr_delta_ratio[j] = snr_delta_nn[j]/snr_delta[j]
    j += 1

yy = np.concatenate((abs(snr_alpha), abs(snr_alpha_nn)))
xx = np.concatenate((np.ones(N_alpha) * 1, np.ones(N_alpha) * 2))
lineLFO = yy.reshape([2, N_alpha])

df = pd.DataFrame({"x": xx, "y": yy})
box_fig = plt.figure('box alpha')
box_fig.add_subplot(111)
flier_props = dict(color='black', markersize=1, linestyle='none', linewidth=0.4)
ax = sns.boxplot(x='x', y='y', data=df, linewidth=0.4, color='black', whis=my_whisk, flierprops=flier_props)
plt.setp(ax.artists, edgecolor='k', facecolor='w')
plt.setp(ax.lines, color='k')
plt.plot(lineLFO, linewidth=0.4, color='green')
plt.yscale('log')
box_fig.savefig('../cppData' + fileName[myData] + '/SNRs_alpha_BoxPlots',
                quality=100, format='eps', bbox_inches='tight')
plt.show()

yy = np.concatenate((abs(snr_delta), abs(snr_delta_nn)))
xx = np.concatenate((np.ones(N_delta) * 1, np.ones(N_delta) * 2))
lineLFO = yy.reshape([2, N_delta])

df = pd.DataFrame({"x": xx, "y": yy})
box_fig = plt.figure('box delta')
box_fig.add_subplot(111)
flier_props = dict(color='black', markersize=1, linestyle='none', linewidth=0.4)
ax = sns.boxplot(x='x', y='y', data=df, linewidth=0.4, color='black', whis=my_whisk, flierprops=flier_props)
plt.setp(ax.artists, edgecolor='k', facecolor='w')
plt.setp(ax.lines, color='k')
plt.plot(lineLFO, linewidth=0.4, color='red')
plt.yscale('log')
box_fig.savefig('../cppData' + fileName[myData] + '/SNRs_delta_BoxPlots',
                quality=100, format='eps', bbox_inches='tight')
plt.show()

box_fig = plt.figure('ratios')
box_fig.add_subplot(111)
plt.plot(snr_alpha_ratio, linewidth=0.4, color='black', marker="s", markersize=4)
plt.plot(snr_delta_ratio, linewidth=0.4, color='blue', marker="o", markersize=4)
plt.xticks(np.arange(0, len(goodOnes), 1))
plt.yscale('log')
box_fig.savefig('../cppData' + fileName[myData] + '/SNRs_ratios',
                quality=100, format='eps', bbox_inches='tight')
plt.show()

box_fig = plt.figure('distribution')
box_fig.add_subplot(111)
plt.scatter(snr_alpha, snr_alpha_ratio, linewidth=0.4, color='blue')
plt.scatter(snr_delta, snr_delta_ratio, linewidth=0.4, color='cyan')
plt.xticks(np.arange(0, len(goodOnes), 1))
plt.yscale('log')
plt.xscale('log')
box_fig.savefig('../cppData' + fileName[myData] + '/distributions',
                quality=100, format='eps', bbox_inches='tight')
plt.show()
