import matplotlib.pyplot as plt
from ParalysedEEG import ParalysedEEG
import numpy as np

p = ParalysedEEG()

for i in range(len(p.allsubjects)):
    x2 = np.linspace(p.f_signal_min,p.f_signal_max,100)
    psd = p.paralysedEEGVarianceFromWhithamEtAl(i)
    plt.plot(x2,psd(x2))
plt.ylim([-16,-10])
plt.xlim([0,100])
plt.xlabel("f/Hz")
plt.ylabel("Log Power (V^2 / Hz)")
plt.show()
