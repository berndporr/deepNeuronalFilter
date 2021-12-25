import matplotlib.pyplot as plt
import numpy as np
import sys

subj = 1

if len(sys.argv) > 1:
    subj = int(sys.argv[1])

fs = 250

def calcVEP(filename,ax):
    p = "../results/subject{}/{}".format(subj,filename)
    print("Loading",p)
    d = np.loadtxt(p)
    y = d[:,0] * 1E6
    tr = d[:,1]
    t = "Subject {}".format(subj)

    oddballs = np.argwhere(tr > 0.5)

    navg = int(fs)
    avg = np.zeros(navg)

    d = 0
    n = 0
    for [ob] in oddballs:
        if (ob+navg) < len(y):
            avg = avg + y[int(ob):int(ob+navg)]
            n = n + 1
            print((ob-d)/fs)
            d = ob            
    avg = avg / n
    t = np.linspace(0,navg/fs,navg) * 1000
    ax.plot(t,avg)
    ax.set_title(filename)
    ax.set_xlabel("t/ms")
    ax.set_ylabel("P300/uV")

fig, axs = plt.subplots(2)
fig.suptitle("P300")
calcVEP("fnn.tsv",axs[0])
calcVEP("inner.tsv",axs[1])

plt.show()
