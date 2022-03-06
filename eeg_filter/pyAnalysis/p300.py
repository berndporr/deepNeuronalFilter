#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
import sys
import getopt

p300dir = "../p300/"

def calcVEP(subj,filename,startsec=30,fs=250):
    p = p300dir+"subject{}/{}".format(subj,filename)
    d = np.loadtxt(p)
    ll = fs * startsec
    y = d[ll:,0]
    tr = d[ll:,-1]
    t = "Subject {}".format(subj)

    oddballs = np.argwhere(tr > 0.5)

    navg = int(fs)
    avg = np.zeros(navg)

    n = 0
    for [ob] in oddballs:
        if (ob+navg) < len(y):
            avg = avg + y[int(ob):int(ob+navg)]
            n = n + 1
    avg = avg / n
    return avg


def plotVEP(subj,filename,ax,fs=250,startsec=10):
    avg = calcVEP(subj,filename,startsec=startsec) * 1E6
    navg = len(avg)
    t = np.linspace(0,navg/fs,navg) * 1000
    ax.plot(t,avg)
    ax.set_title(filename)
    ax.set_xlabel("t/ms")
    ax.set_ylabel("P300/uV")


# check if we run this as a main program
if __name__ == "__main__":
    subj = 1
    startsec = 120
    filtered_filename = "dnf.tsv"

    helptext = 'usage: {} -p participant -s startsec -f file -h'.format(sys.argv[0])

    try:
        # Gather the arguments
        all_args = sys.argv[1:]
        opts, arg = getopt.getopt(all_args, 'p:s:f:')
        # Iterate over the options and values
        for opt, arg_val in opts:
            if '-p' in opt:
                subj = int(arg_val)
            elif '-s' in opt:
                startsec = int(arg_val)
            elif '-f' in opt:
                filtered_filename = arg_val
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        sys.exit(2)

    fig, axs = plt.subplots(2)
    fig.suptitle("P300")
    plotVEP(subj,filtered_filename,axs[0],startsec=startsec)
    plotVEP(subj,"inner.tsv",axs[1],startsec=startsec)

    plt.show()
