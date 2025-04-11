import numpy as np
import h5py
import matplotlib.pyplot as plt

fs = 2.5E9
t0 = 1.5E-5
sweepNo = 300
a = h5py.File('data_test_pmma_laser_strips_fine_step8.mat')
print(list(a.keys()))
dset = a['data']
print(dset.shape)
a0 = int(t0*fs)
d = dset[a0:,sweepNo]
t = np.linspace(0,len(d)/fs,len(d))
plt.plot(t,d)
plt.xlabel("time/sec")
plt.ylabel("amplitude")
fy = np.fft.fft(d)
plt.figure()
fx = np.linspace(0,fs,len(fy))
plt.plot(fx,np.abs(fy)/len(fy))
plt.xlim([0,20E6])
plt.xlabel("frequ/Hz")
plt.ylabel("amplitude")
plt.show()
