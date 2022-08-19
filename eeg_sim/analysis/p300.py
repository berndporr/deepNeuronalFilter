#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
import sys
import getopt

p300dir = "../p300/"

def calcVEP(subj,filename,startsec,fs):
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


def plotVEP(subj,filename,ax,fs,startsec):
    avg = calcVEP(subj,filename,startsec=startsec,fs=fs) * 1E6
    navg = len(avg)
    t = np.linspace(0,navg/fs,navg) * 1000
    ax.plot(t,avg)
    ax.set_title(filename)
    ax.set_xlabel("t/ms")
    ax.set_ylabel("P300/uV")


# check if we run this as a main program
if __name__ == "__main__":
    subj = 1
    startsec = 2*60
    dnf_filename = "dnf.tsv"
    lms_filename = "lms.tsv"
    laplace_filename = "laplace.tsv"
    fs = 250

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
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        sys.exit(2)

    fig = plt.figure("P300")

    ax = fig.add_subplot(1,4,1)
    plotVEP(subj,"inner.tsv",ax,startsec=startsec,fs=fs)

    ax = fig.add_subplot(1,4,2)
    ax.title.set_text('DNF')
    plotVEP(subj,dnf_filename,ax,startsec=startsec,fs=fs)

    ax = fig.add_subplot(1,4,3)
    ax.title.set_text('LMS')
    plotVEP(subj,lms_filename,ax,startsec=startsec,fs=fs)

    ax = fig.add_subplot(1,4,4)
    ax.title.set_text('Laplace')
    plotVEP(subj,laplace_filename,ax,startsec=startsec,fs=fs)

    plt.show()
