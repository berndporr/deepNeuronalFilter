import matplotlib.pyplot as plt
import numpy as np
import sys
import p300
import scipy.signal as signal
import getopt


# http://www.scholarpedia.org/article/Signal-to-noise_ratio_in_neuroscience

def hpFilter(x,fs=250):
    fc = 10
    b,a = signal.butter(2,fc/fs*2,"highpass")
    return signal.lfilter(b,a,x)

def calcNoisePower(subj,filename,colmn,startsec=10,fs=250,folder="results"):
    p = "../{}/subject{}/{}".format(folder,subj,filename)
    d = np.loadtxt(p)
    ll = fs * startsec
    y = d[ll:,colmn]
    s = 0
    freq, power = signal.periodogram(y,fs,scaling="spectrum")
    for f,p in zip(freq, power):
        if (f > 5) and (f < 75):
            s = s + p
    return s

def calcSNRinner(subj,startsec=10,fs=250,folder="results"):
    filename = "inner.tsv"
    NoisePwr = calcNoisePower(subj,filename,0,startsec,fs,folder=folder)
    vep = p300.calcVEP(subj,filename,startsec,fs)
    SignalPwr = np.mean(vep[int(fs*0.4):]**2)
    print("Signal Power:",SignalPwr)
    print("NoisePwr:",NoisePwr)
    snr = SignalPwr/NoisePwr
    return snr

def calcSNRdnf(subj,filename,startsec=10,fs=250,folder="results"):
    NoisePwr = calcNoisePower(subj,filename,0,startsec,fs,folder=folder)
    vep = p300.calcVEP(subj,filename,startsec,fs)
    SignalPwr = np.mean(vep[int(fs*0.4):]**2)
    print("Signal Power:",SignalPwr)
    print("NoisePwr:",NoisePwr)
    snr = SignalPwr/NoisePwr
    return snr

# check if we run this as a main program
if __name__ == "__main__":
    subj = 1
    startsec = 120
    noisefolder = "results"
    filtered_filename = "fnn.tsv"

    helptext = 'usage: {} -p participant -s startsec -f file -n noisefolder -h'.format(sys.argv[0])

    try:
        # Gather the arguments
        all_args = sys.argv[1:]
        opts, arg = getopt.getopt(all_args, 'p:s:f:n:')
        # Iterate over the options and values
        for opt, arg_val in opts:
            if '-p' in opt:
                subj = int(arg_val)
            elif '-s' in opt:
                startsec = int(arg_val)
            elif '-f' in opt:
                filtered_filename = arg_val
            elif '-n' in opt:
                noisefolder = arg_val
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        sys.exit(2)

    print("SNR from Noise removal:",calcSNRdnf(subj,filtered_filename,startsec=startsec,folder=noisefolder))
    print()
    print()
    print("SNR just from inner:",calcSNRinner(subj,startsec=startsec,folder=noisefolder))

    plt.show()
