import matplotlib.pyplot as plt
from ParalysedEEG import ParalysedEEG
import numpy as np
from scipy import signal

filter_order = 2
cutoff = 17 # Hz
eeg_max_signal_amplitude = 1.5e-6 # Volt / Hz

b, a = signal.butter(filter_order, cutoff, 'low', analog=True)
w, h = signal.freqs(b, a)
h = h * eeg_max_signal_amplitude

for i in range(len(ParalysedEEG.allsubjectdata)):
    p = ParalysedEEG(i)
    f = np.linspace(p.f_signal_min,p.f_signal_max,100)
    psd = p.paralysedEEGVarianceFromWhithamEtAl(f)
    totalEEGPower = p.totalEEGPower()
    plt.plot(f,psd,linewidth=1,label="dataset #{}".format(i))
plt.ylim([-16,-10])
plt.xlim([0,100])
plt.xlabel("f/Hz")
plt.ylabel("Log Power (V^2 / Hz)")
plt.plot(w, 2*np.log10(abs(h)), linewidth=3, color="0", label='2nd Order Butterworth fc={}, order={}'.format(cutoff,filter_order))
plt.legend()
plt.show()
