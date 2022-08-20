import matplotlib.pyplot as plt
from ParalysedEEG import ParalysedEEG
import numpy as np

for i in range(len(ParalysedEEG.allsubjectdata)):
    p = ParalysedEEG(i)
    f = np.linspace(p.f_signal_min,p.f_signal_max,100)
    psd = p.paralysedEEGVarianceFromWhithamEtAl(f)
    totalEEGPower = p.totalEEGPower()
    print("Average EEG voltage is (uV):",round((totalEEGPower**0.5)*1E6))
    plt.plot(f,psd,linewidth=1,label="dataset #{}".format(i))
p = ParalysedEEG()
f = np.linspace(p.f_signal_min,p.f_signal_max,100)
psd = p.paralysedEEGVarianceFromWhithamEtAl(f)
totalEEGPower = p.totalEEGPower()
print("Average EEG voltage is (uV):",round((totalEEGPower**0.5)*1E6))
plt.ylim([-16,-10])
plt.xlim([0,100])
plt.xlabel("f/Hz")
plt.ylabel("Log Power (V^2 / Hz)")
plt.plot(f,psd,linewidth=3,color="0",label="Average")
plt.legend()
plt.show()
