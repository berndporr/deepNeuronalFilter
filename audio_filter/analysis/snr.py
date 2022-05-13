#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
import sys
import scipy.signal as signal
import getopt
import os
from scipy.io import wavfile

SNRbandMin = 20 # Hz
SNRbandMax = 10000 # Hz

endsec = 5

signal_noise_filename = "signal_noise.dat"
noise_filename = "noise.dat"
signal_filename = "signal.wav"

class SNR:
    def __init__(self,experiment,fs,noisered_filename):
        self.experiment = experiment
        self.fs = fs
        self.noisered_filename = noisered_filename

    def loadResultsFile(self,filename):
        p = "../results/exp{}/{}".format(self.experiment,filename)
        d = np.loadtxt(p,comments=';')
        return d[int(self.startsec*self.fs):,1]

    def loadOrigWAV(self,filename):
        p = "../audio/exp{}/{}".format(self.experiment,filename)
        samplerate, data = wavfile.read(p)
        left = data[:, 0]
        right = data[:, 1]
        return left,right

    def calcSpectrum(self,y):
        s = 0
        freq, power = signal.welch(y,self.fs,scaling="spectrum",nperseg=self.fs)
        w = np.array([])
        for f,p in zip(freq, power):
            if (f >= SNRbandMin) and (f <= SNRbandMax):
                s = s + p
                if not w.any():
                    w = np.array([f,p])
                else:
                    w = np.row_stack((w,np.array([f,p])))
        return s,w

    def calcSNRbefore(self):
        s = self.loadOrigWAV(signal_filename)
        SignalPwr = np.var(s)
        print("Signal Power:",SignalPwr)
        n = self.loadOrigWAV(noise_filename)
        NoisePwr = np.var(n)
        print("NoisePwr:",NoisePwr)
        snr = np.log10(SignalPwr/NoisePwr)*10
        return snr

    def calcSNRafter(self):
        s = self.loadOrigWAV(signal_filename)
        SignalPwr = np.var(s)
        print("Signal Power:",SignalPwr)
        n = self.loadResultsFile(signal_noise_filename)
        NoisePwr = np.var(n)
        print("NoisePwr:",NoisePwr)
        snr = np.log10(SignalPwr/NoisePwr)*10
        return snr

    
# check if we run this as a main program
if __name__ == "__main__":
    subj = 1
    startsec = 1
    fs = 48000
    filtered_filename = "dnf.tsv"

    helptext = 'usage: {} -p experiment -f file -h'.format(sys.argv[0])

    try:
        # Gather the arguments
        all_args = sys.argv[1:]
        opts, arg = getopt.getopt(all_args, 'p:f:h')
        # Iterate over the options and values
        for opt, arg_val in opts:
            if '-p' in opt:
                experiment = int(arg_val)
            elif '-f' in opt:
                filtered_filename = arg_val
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        sys.exit(2)

    plt.figure("Periodogram of the noise: unfilered vs filtered")
    snr = SNR(experiment,fs,filtered_filename)
    snrdnf = snr.calcSNRdnf()
    print("SNR from Noise removal:",snrdnf)
    plt.plot(wdnf[:,0],wdnf[:,1],label=filtered_filename)
    plt.legend()
    print()
    print()
    snrinner, winner = snr.calcSNRinner()
    print("SNR just from inner:",snrinner)
    plt.plot(winner[:,0],winner[:,1],label="INNER")
    plt.ylabel("V^2/Hz")
    plt.xlabel("Hz")
    plt.legend()

    plt.show()
