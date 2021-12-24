import matplotlib.pyplot as plt
import numpy as np
import sys

def plotTimeSpectr(y,title):
    fs = 250
    fig, axs = plt.subplots(2)
    fig.suptitle(title)
    axs[0].plot(y1);
    #
    # Fourier Transform
    yf = np.fft.fft(y) / len(y)
    yf[0] = 0
    axs[1].plot(np.linspace(0,fs,len(yf)),abs(yf))

subj = 1

if len(sys.argv) > 1:
    subj = int(sys.argv[1])
    
p = "../../noisewalls/EEG_recordings/participant{:03d}/rawp300.tsv".format(subj)
d = np.loadtxt(p)
y1 = d[:,0]
y2 = d[:,1]
t = "Subject {}".format(subj)
plotTimeSpectr(y1,t+", Channel 1")
plotTimeSpectr(y2,t+", Channel 2")
plt.show()
