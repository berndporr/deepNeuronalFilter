import matplotlib.pyplot as plt
import numpy as np
import sys


def calcVEP(subj,filename,startSec=60,fs=250):
    p = "../results/subject{}/{}".format(subj,filename)
    print("Loading",p)
    d = np.loadtxt(p)
    ll = fs * startSec
    y = d[ll:,0]
    tr = d[ll:,1]
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
    return avg

    
def plotVEP(subj,filename,ax,fs=250):
    avg = calcVEP(subj,filename) * 1E6
    navg = len(avg)
    t = np.linspace(0,navg/fs,navg) * 1000
    ax.plot(t,avg)
    ax.set_title(filename)
    ax.set_xlabel("t/ms")
    ax.set_ylabel("P300/uV")


# check if we run this as a main program
if __name__ == "__main__":
    subj = 1

    if len(sys.argv) > 1:
        subj = int(sys.argv[1])

    fig, axs = plt.subplots(2)
    fig.suptitle("P300")
    plotVEP(subj,"fnn.tsv",axs[0])
    plotVEP(subj,"inner.tsv",axs[1])

    plt.show()
