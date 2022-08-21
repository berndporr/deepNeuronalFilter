#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
import sys
import getopt

from scipy import signal

resultsdir = "../results/"

def calcVEP(subj,filename,startsec,fs):
    d = np.loadtxt(resultsdir+"/"+str(subj)+"/"+filename)
    ll = fs * startsec
    y1 = d[ll:,0]
    s = np.loadtxt(resultsdir+"/"+str(subj)+"/signal.tsv")
    y2 = s[ll:]
    avg = signal.correlate(y1,y2)
    avg = np.abs(avg)
    return avg


def plotVEP(subj,filename,ax,fs,startsec):
    avg = calcVEP(subj,filename,startsec=startsec,fs=fs) * 1E6
    navg = len(avg)
    t = np.linspace(-navg/fs/2,navg/fs/2,navg)
    ax.plot(t,avg)
    ax.set_title(filename)
    ax.set_xlabel("t/ms")
    ax.set_ylabel("P300/uV")


# check if we run this as a main program
if __name__ == "__main__":
    subj = 0
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
