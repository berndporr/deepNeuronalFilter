import numpy as np
import h5py
import matplotlib.pyplot as plt
import sys

fs = 2.5E9
t0 = 1.5E-5
if len(sys.argv) < 3:
    print("Usage: {} datafile.dat col".format(sys.argv[0]))
    quit(1)
data = np.loadtxt(sys.argv[1])
d = data[:,int(sys.argv[2])]
t = np.linspace(0,len(d)/fs,len(d))
plt.plot(t,d)
plt.xlabel("time/sec")
plt.ylabel("amplitude")
fy = np.fft.fft(d)
plt.figure()
fx = np.linspace(0,fs,len(fy))/1E6
plt.plot(fx,np.abs(fy)/len(fy))
plt.xlabel("frequ/MHz")
plt.ylabel("amplitude")
plt.xlim([0,20])
plt.show()
