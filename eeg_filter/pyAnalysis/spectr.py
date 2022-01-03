import matplotlib.pyplot as plt
import numpy as np
import sys


def plotSpectr(subj,filename,ax,fs=250,startsec=10):
    p = "../results/subject{}/{}".format(subj,filename)
    d = np.loadtxt(p)
    ll = fs * startsec
    y = d[ll:,0]
    t = "Subject {} {}".format(subj,filename)
    f = np.linspace(0,fs,len(y))
    yf = np.fft.fft(y)
    yf[0] = 0
    yf = np.abs(yf) / len(yf)
    ax.semilogy(f,yf)
    ax.set_title(t)
    ax.set_xlabel("f/Hz")
    ax.set_ylabel("P300/mV")
    ax.set_xlim([0,fs/2])


# check if we run this as a main program
if __name__ == "__main__":
    subj = 1

    if len(sys.argv) > 1:
        subj = int(sys.argv[1])

    startsec = 120

    fig, axs = plt.subplots(2)
    fig.suptitle("P300")
    plotSpectr(subj,"fnn.tsv",axs[0],startsec=startsec)
    plotSpectr(subj,"inner.tsv",axs[1],startsec=startsec)

    plt.show()
