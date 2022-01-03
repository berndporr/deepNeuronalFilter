import matplotlib.pyplot as plt
import numpy as np
import sys
import p300
import scipy.signal as signal

# http://www.scholarpedia.org/article/Signal-to-noise_ratio_in_neuroscience

def hpFilter(x,fs=250):
    fc = 10
    b,a = signal.butter(2,fc/fs*2,"highpass")
    return signal.lfilter(b,a,x)

def calcNoisePower(subj,filename,startsec=10,fs=250,folder="results"):
    p = "../{}/subject{}/{}".format(folder,subj,filename)
    d = np.loadtxt(p)
    ll = fs * startsec
    y = d[ll:,0]
    s = np.std(y)
    print("Noise Power:",s)
    return s

def calcSNR(subj,filename,startsec=10,fs=250,folder="results"):
    NoisePwr = calcNoisePower(subj,filename,startsec,fs,folder=folder)
    vep = p300.calcVEP(subj,filename,startsec,fs)
    s = np.max(vep)
    SignalPwr = s*s
    print("Signal Power:",SignalPwr)
    snr = SignalPwr/NoisePwr
    return snr

# check if we run this as a main program
if __name__ == "__main__":
    subj = 1

    if len(sys.argv) > 1:
        subj = int(sys.argv[1])

    startsec = 120
    folder = "sudoku"

    print("SNR from DNF:",calcSNR(subj,"fnn.tsv",startsec=startsec,folder=folder))
    print("SNR just from inner:",calcSNR(subj,"inner.tsv",startsec=startsec,folder=folder))

    plt.show()
