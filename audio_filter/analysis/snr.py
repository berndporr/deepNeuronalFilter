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

noise_filename = "noise.wav"
signal_filename = "signal.wav"

class SNR:
    def __init__(self,experiment,fs,noisered_filename):
        self.experiment = experiment
        self.fs = fs
        self.noisered_filename = noisered_filename

    def loadResultsFile(self,filename,startsec=0,endsec=False):
        p = "../results/exp{}/{}".format(self.experiment,filename)
        d = np.loadtxt(p,comments=';')
        if not endsec:
            return d[int(startsec*self.fs):,1]
        else:
            return d[int(startsec*self.fs):int(endsec*self.fs),1]

    def loadOrigWAV(self,filename):
        p = "../audio/exp{}/{}".format(self.experiment,filename)
        samplerate, data = wavfile.read(p)
        signal_noise = data[:, 0] / 32768 # left channel
        return signal_noise

    def calcSpectrum(self,y):
        freq, power = signal.welch(y,self.fs,scaling="spectrum",nperseg=self.fs)
        w = np.array([])
        for f,p in zip(freq, power):
            if (f >= SNRbandMin) and (f <= SNRbandMax):
                if not w.any():
                    w = np.array([f,p])
                else:
                    w = np.row_stack((w,np.array([f,p])))
        return w

    def calcSpectrumBefore(self):
        n = self.loadOrigWAV(noise_filename)
        return self.calcSpectrum(n)

    def calcSpectrumAfter(self):
        n = self.loadResultsFile(self.noisered_filename,-endsec)
        return self.calcSpectrum(n)

    def calcSNRbefore(self):
        s = self.loadOrigWAV(signal_filename)
        SignalPwr = np.var(s)
        print("Signal Power:",SignalPwr)
        n = self.loadOrigWAV(noise_filename)
        NoisePwr = np.var(n)
        print("NoisePwr:",NoisePwr)
        nCheck = self.loadResultsFile(self.noisered_filename,0,endsec)
        NoisePwrCheck = np.var(nCheck)
        print("NoisePwr sanity check:",NoisePwrCheck)
        snr = np.log10(SignalPwr/NoisePwr)*10
        return snr

    def calcSNRafter(self):
        s = self.loadOrigWAV(signal_filename)
        SignalPwr = np.var(s)
        print("Signal Power:",SignalPwr)
        n = self.loadResultsFile(self.noisered_filename,-endsec)
        NoisePwr = np.var(n)
        print("NoisePwr:",NoisePwr)
        snr = np.log10(SignalPwr/NoisePwr)*10
        return snr

    
# check if we run this as a main program
if __name__ == "__main__":
    experiment = 1
    startsec = 1
    fs = 48000
    filtered_filename = "dnf_out.dat"

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
    snrbefore = snr.calcSNRbefore()
    print("SNR before Noise removal:",snrbefore)
    snrafter = snr.calcSNRafter()
    print("SNR from Noise removal:",snrafter)
    w1 = snr.calcSpectrumAfter()
    plt.semilogx(w1[:,0],w1[:,1],label=filtered_filename)
    plt.legend()
    print()
    print()
    w2 = snr.calcSpectrumBefore()
    plt.semilogx(w2[:,0],w2[:,1],label="before")
    plt.ylabel("V^2/Hz")
    plt.xlabel("Hz")
    plt.legend()

    plt.show()
