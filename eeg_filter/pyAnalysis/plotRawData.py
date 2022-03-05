#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed May 13 00:33:19 2020

@author: Sama
"""
# !/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pylab as plt
import scipy.signal as sig
import fir1

plt.rcParams['font.family'] = 'serif'
plt.rcParams.update({'font.size': 5})

plt.close("all")
fs = 250

subject = 14

originalEEGdataCLOSED = np.loadtxt('../../SubjectData_EyesClosed_Novel/EyesClosedNovel_Subject-{}.tsv'.format(subject))
originalEEGdataOPEN = np.loadtxt('../../SubjectData_EyesOpen_Novel/EyesOpenNovel_Subject-{}.tsv'.format(subject))
data_length = min(len(originalEEGdataCLOSED), len(originalEEGdataOPEN))
innerElectSignal_CLOSED = originalEEGdataCLOSED[0:data_length, 1]
outerElectNoise_CLOSED = originalEEGdataCLOSED[0:data_length, 2]
innerElectSignal_OPEN = originalEEGdataOPEN[0:data_length, 1]
outerElectNoise_OPEN = originalEEGdataOPEN[0:data_length, 2]

print(max(innerElectSignal_CLOSED))
print(max(outerElectNoise_CLOSED))
print(max(innerElectSignal_OPEN))
print(max(outerElectNoise_OPEN))

freqXRaw_CLOSED = np.linspace(0, fs/2, int(data_length/2))
freqXRaw_OPEN = np.linspace(0, fs/2, int(data_length/2))

fft_innerElectSignal_CLOSED = np.abs(np.fft.fft(innerElectSignal_CLOSED)[0:np.int(data_length/2)])
fft_outerElectNoise_CLOSED = np.abs(np.fft.fft(outerElectNoise_CLOSED)[0:np.int(data_length/2)])

fft_innerElectSignal_OPEN = np.abs(np.fft.fft(innerElectSignal_OPEN)[0:np.int(data_length/2)])
fft_outerElectNoise_OPEN = np.abs(np.fft.fft(outerElectNoise_OPEN)[0:np.int(data_length/2)])

dc = int((0.5/fs)*data_length)
low50 = int((48/fs)*data_length)
high50 = int((52/fs)*data_length)

fft_innerElectSignal_CLOSED[0:dc] = 0
fft_outerElectNoise_CLOSED[0:dc] = 0
fft_innerElectSignal_CLOSED[low50:high50] = 0
fft_outerElectNoise_CLOSED[low50:high50] = 0

fft_innerElectSignal_OPEN[0:dc] = 0
fft_outerElectNoise_OPEN[0:dc] = 0
fft_innerElectSignal_OPEN[low50:high50] = 0
fft_outerElectNoise_OPEN[low50:high50] = 0

myFig0 = plt.figure('raw fig')
ax1 = myFig0.add_subplot(421)
plt.plot(outerElectNoise_OPEN, linewidth=0.4)
plt.title('raw outer eyes open')
ax2 = myFig0.add_subplot(422)
plt.plot(freqXRaw_OPEN, fft_outerElectNoise_OPEN, linewidth=0.4)

ax3 = myFig0.add_subplot(423)
plt.plot(innerElectSignal_OPEN, linewidth=0.4)
plt.title('raw inner eyes open')
ax4 = myFig0.add_subplot(424)
plt.plot(freqXRaw_OPEN, fft_innerElectSignal_OPEN, linewidth=0.4)

ax5 = myFig0.add_subplot(425)
plt.plot(outerElectNoise_CLOSED, linewidth=0.4)
plt.title('raw outer eyes closed')
ax6 = myFig0.add_subplot(426)
plt.plot(freqXRaw_CLOSED, fft_outerElectNoise_CLOSED, linewidth=0.4)

ax7 = myFig0.add_subplot(427)
plt.plot(innerElectSignal_CLOSED, linewidth=0.4)
plt.title('raw inner eyes closed')
ax8 = myFig0.add_subplot(428)
plt.plot(freqXRaw_CLOSED, fft_innerElectSignal_CLOSED, linewidth=0.4)

fs = 250
cf = (np.arange(0, 126, 1)/fs)*2
filterDelay = 999
DC_removal = (sig.firwin(filterDelay, [cf[2]], window='hanning', pass_zero=False))*1000
DC_removal_outer_open = fir1.Fir1(DC_removal)
DC_removal_inner_open = fir1.Fir1(DC_removal)
DC_removal_outer_closed = fir1.Fir1(DC_removal)
DC_removal_inner_closed = fir1.Fir1(DC_removal)

flat_outer_open = np.zeros(data_length)
flat_inner_open = np.zeros(data_length)
flat_outer_closed = np.zeros(data_length)
flat_inner_closed = np.zeros(data_length)

for i in range(data_length):
    flat_outer_open[i] = DC_removal_outer_open.filter(outerElectNoise_OPEN[i])
    flat_inner_open[i] = DC_removal_inner_open.filter(innerElectSignal_OPEN[i])
    flat_outer_closed[i] = DC_removal_outer_closed.filter(outerElectNoise_CLOSED[i])
    flat_inner_closed[i] = DC_removal_inner_closed.filter(innerElectSignal_CLOSED[i])

flat_outer_open = flat_outer_open[filterDelay::]
flat_inner_open = flat_inner_open[filterDelay::]
flat_outer_closed = flat_outer_closed[filterDelay::]
flat_inner_closed = flat_inner_closed[filterDelay::]

dataLengthShort = len(flat_outer_open)

freqXDC_OPEN = np.linspace(0, fs/2, int(dataLengthShort/2))
freqXDC_CLOSED = np.linspace(0, fs/2, int(dataLengthShort/2))
freqCutDC = 1
fft_flat_outer_open = np.abs(np.fft.fft(flat_outer_open)[0:np.int(dataLengthShort/2)])
fft_flat_inner_open = np.abs(np.fft.fft(flat_inner_open)[0:np.int(dataLengthShort/2)])
fft_flat_outer_closed = np.abs(np.fft.fft(flat_outer_closed)[0:np.int(dataLengthShort/2)])
fft_flat_inner_closed = np.abs(np.fft.fft(flat_inner_closed)[0:np.int(dataLengthShort/2)])

dc_flat = int((0/fs)*dataLengthShort)
low50_flat = int((48/fs)*dataLengthShort)
high50_flat = int((52/fs)*dataLengthShort)

fft_flat_inner_closed[0:dc_flat] = 0
fft_flat_outer_closed[0:dc_flat] = 0
fft_flat_inner_closed[low50_flat:high50_flat] = 0
fft_flat_outer_closed[low50_flat:high50_flat] = 0

fft_flat_inner_open[0:dc_flat] = 0
fft_flat_outer_open[0:dc_flat] = 0
fft_flat_inner_open[low50_flat:high50_flat] = 0
fft_flat_outer_open[low50_flat:high50_flat] = 0

myFig0 = plt.figure('DC removed fig')
ax1 = myFig0.add_subplot(421)
plt.plot(flat_outer_open, linewidth=0.4)
plt.title('outer eyes open')
ax2 = myFig0.add_subplot(422)
plt.plot(freqXDC_OPEN[freqCutDC::], fft_flat_outer_open[freqCutDC::], linewidth=0.4)

ax3 = myFig0.add_subplot(423)
plt.plot(flat_inner_open, linewidth=0.4)
plt.title('inner eyes open')
ax4 = myFig0.add_subplot(424)
plt.plot(freqXDC_OPEN[freqCutDC::], fft_flat_inner_open[freqCutDC::], linewidth=0.4)

ax5 = myFig0.add_subplot(425)
plt.plot(flat_outer_closed, linewidth=0.4)
plt.title('outer eyes closed')
ax6 = myFig0.add_subplot(426)
plt.plot(freqXDC_CLOSED[freqCutDC::], fft_flat_outer_closed[freqCutDC::], linewidth=0.4)

ax7 = myFig0.add_subplot(427)
plt.plot(flat_inner_closed, linewidth=0.4)
plt.title('inner eyes closed')
ax8 = myFig0.add_subplot(428)
plt.plot(freqXDC_CLOSED[freqCutDC::], fft_flat_inner_closed[freqCutDC::], linewidth=0.4)
