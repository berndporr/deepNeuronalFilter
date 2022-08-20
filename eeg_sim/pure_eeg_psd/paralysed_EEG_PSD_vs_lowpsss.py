import matplotlib.pyplot as plt
from ParalysedEEG import ParalysedEEG
import numpy as np
from scipy import signal

b, a = signal.butter(2, 5, 'low', analog=True)
w, h = signal.freqs(b, a)
print(w)
print(h)

p = ParalysedEEG()

for i in range(len(p.allsubjectdata)):
    x2 = np.linspace(p.f_signal_min,p.f_signal_max,100)
    psd = p.paralysedEEGVarianceFromWhithamEtAl(i)
    plt.plot(x2,psd(x2))
plt.plot(w, 2*np.log10(abs(h*7.5e-6)))
plt.ylim([-16,-10])
plt.xlim([0,100])
plt.xlabel("f/Hz")
plt.ylabel("Log Power (V^2 / Hz)")
plt.show()
