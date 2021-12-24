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

fs = 250
p = "../results/subject{}/fnn.tsv".format(subj)
d = np.loadtxt(p)
y = d[:,0]
tr = d[:,1]
t = "Subject {}".format(subj)

oddballs = np.argwhere(tr > 0.5)

navg = int(fs)
avg = np.zeros(navg)

d = 0
for [ob] in oddballs:
    if (ob+navg) < len(y):
        avg = avg + y[int(ob):int(ob+navg)]
        print(ob-d)
        d = ob

avg = avg / len(oddballs)

plt.plot(avg)

plt.show()
