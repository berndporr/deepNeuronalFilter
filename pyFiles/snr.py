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

def calcNoisePower(subj,filename,startSec=60,fs=250):
    p = "../sudoku/subject{}/{}".format(subj,filename)
    d = np.loadtxt(p)
    ll = fs * startSec
    y = d[ll:,0]
    s = np.std(y)
    print("Noise Power:",s)
    return s

def calcSNR(subj,filename,startSec=60,fs=250):
    NoisePwr = calcNoisePower(subj,filename,startSec,fs)
    vep = p300.calcVEP(subj,filename,startSec,fs)
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

    print("SNR from DNF:",calcSNR(subj,"fnn.tsv"))
    print("SNR just from inner:",calcSNR(subj,"inner.tsv"))

    plt.show()
