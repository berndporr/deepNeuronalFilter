#!/usr/bin/python3
import matplotlib.pyplot as plt
from nmb_eeg import NMB_EEG_From_WhithamEtAl
import numpy as np
from scipy import signal

filter_order = 2
cutoff = 17 # Hz
eeg_max_signal_amplitude = 1.5e-6 # Volt / Hz

for i in range(len(NMB_EEG_From_WhithamEtAl.allsubjectdata)):
    p = NMB_EEG_From_WhithamEtAl(i)
    f = np.linspace(p.f_signal_min,p.f_signal_max,100)
    psd = p.EEGVariance(f)
    plt.plot(f,psd,linewidth=1,label="dataset #{}".format(i))
plt.ylim([-16,-9])
plt.xlim([0,100])
plt.xlabel("f/Hz")
plt.ylabel("Log Power (V^2 / Hz)")
st = ['dashed',
      'solid',
      'dotted']
for filter_order in range(1,4):
    b, a = signal.butter(filter_order, cutoff, 'low', analog=True)
    w, h = signal.freqs(b, a)
    h = h * eeg_max_signal_amplitude
    plt.plot(w, 2*np.log10(abs(h)), linewidth=3, color="0", label='2nd Order Butterworth fc={}Hz, order={}'.format(cutoff,filter_order),linestyle=st[filter_order-1])
plt.legend()
plt.show()
