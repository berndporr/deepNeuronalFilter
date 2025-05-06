import numpy as np
import matplotlib.pyplot as plt
import sys

fs = 1000

data = np.loadtxt("ecg_filtered.dat")

col = 0

if len(sys.argv) > 1:
    col = int(sys.argv[1])

d = data[:,col]

t = np.linspace(0,len(d)/fs,len(d))
plt.plot(t,d)
plt.xlabel("time/sec")
plt.ylabel("amplitude")
fy = np.fft.fft(d)
plt.figure()
fx = np.linspace(0,fs,len(fy))
plt.semilogy(fx,np.abs(fy)/len(fy))
plt.xlabel("frequ/Hz")
plt.ylabel("amplitude")
plt.ylim([0,0.1])
plt.xlim([0,500])
plt.show()
