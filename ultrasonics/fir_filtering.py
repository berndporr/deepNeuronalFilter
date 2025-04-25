import numpy as np
from scipy.signal import kaiserord, firwin, freqz
import matplotlib.pyplot as plt
fs = 2.5E9
cutoff = 10E6
width = 0.8E6
numtaps, beta = kaiserord(60, width/(0.5*fs))
print(numtaps)
coeff = firwin(numtaps, [cutoff-width//2,cutoff+width//2], window=('kaiser', beta), scale=False, fs=fs, pass_zero=False)
w, h = freqz(coeff, worN=8000)
w *= 0.5*fs/np.pi  # Convert w to Hz.
plt.plot(w,np.abs(h))
plt.show()
