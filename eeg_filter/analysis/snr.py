#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
import sys
import p300
import scipy.signal as signal
import getopt
import os

subjectsOK = [1,3,4,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]

SNRbandMin = 5 # Hz
SNRbandMax = 100 # Hz

VEPstartTime = 0.3 # sec
VEPendTime = 0.5 # sec

p300task = "p300"

class SNR:
    def __init__(self,subj,startsec,fs,folder,noisered_filename):
        self.subj = subj
        self.startsec = startsec
        self.fs = fs
        self.folder = folder
        self.noisered_filename = noisered_filename
        self.inner_filename = "inner.tsv"
        self.outer_filename = "outer.tsv"
    
    def loadSignal(self,filename):
        p = "../{}/subject{}/{}".format(self.folder,self.subj,filename)
        d = np.loadtxt(p)
        ll = self.fs * self.startsec
        y = d[ll:,0]
        return y
        
    def calcNoisePower(self,filename):
        y = self.loadSignal(filename)
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

    def calcSNRinner(self):
        NoisePwr,w = self.calcNoisePower(self.inner_filename)
        vep = p300.calcVEP(self.subj,self.inner_filename,self.startsec,self.fs)
        SignalPwr = np.median(vep[int(self.fs*VEPstartTime):int(self.fs*VEPendTime)]**2)
        print("Signal Power:",SignalPwr)
        print("NoisePwr:",NoisePwr)
        snr = np.log10(SignalPwr/NoisePwr)*10
        return snr,w

    def calcSNRdnf(self):
        NoisePwr,w = self.calcNoisePower(self.noisered_filename)
        vep = p300.calcVEP(self.subj,self.noisered_filename,self.startsec,self.fs)
        SignalPwr = np.median(vep[int(self.fs*VEPstartTime):int(self.fs*VEPendTime)]**2)
        print("Signal Power:",SignalPwr)
        print("NoisePwr:",NoisePwr)
        snr = np.log10(SignalPwr/NoisePwr)*10
        return snr,w

    def calcCrossCorr(self):
        inner = self.loadSignal(self.inner_filename)
        outer = self.loadSignal(self.outer_filename)
        lags = signal.correlation_lags(len(inner), len(outer)) / self.fs
        cc = signal.correlate(inner,outer)
        return (lags,cc)

    
def calcAllSNRimprovemements(startsec,
                             noisefolder,
                             fs,
                             filtered_filename):
    beforeArray = np.array([])
    afterArray = np.array([])
    for subj in subjectsOK:
        print("Subject",subj)
        snr = SNR(subj=subj,startsec=startsec,fs=fs,folder=noisefolder,noisered_filename=filtered_filename)
        snrdnf, wdnf = snr.calcSNRdnf()
        snrinner, winner = snr.calcSNRinner()
        impr = snrdnf-snrinner
        print("SNR improvement: {} - {} = {}".format(snrinner,snrdnf,impr))
        beforeArray = np.append(beforeArray,snrinner)
        afterArray = np.append(afterArray,snrdnf)
    imprArray = afterArray - beforeArray
    snrdiff_av = np.mean(imprArray)
    snrdiff_sd = np.std(imprArray)
    return beforeArray,afterArray,snrdiff_av,snrdiff_sd


# check if we run this as a main program
if __name__ == "__main__":
    subj = 1
    startsec = 60
    taskfolder = p300task
    fs = 250
    filtered_filename = "dnf.tsv"

    helptext = 'usage: {} -p participant -s startsec -f file -t task -h'.format(sys.argv[0])

    try:
        # Gather the arguments
        all_args = sys.argv[1:]
        opts, arg = getopt.getopt(all_args, 'p:s:f:t:')
        # Iterate over the options and values
        for opt, arg_val in opts:
            if '-p' in opt:
                subj = int(arg_val)
            elif '-s' in opt:
                startsec = int(arg_val)
            elif '-f' in opt:
                filtered_filename = arg_val
            elif '-t' in opt:
                if p300task != arg_val:
                    fs = 500
                    taskfolder = arg_val
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        sys.exit(2)

    plt.figure("Periodogram of the noise: unfilered (INNER) vs filtered (.tsv)")
    snr = SNR(subj=subj,startsec=startsec,fs=fs,folder=taskfolder,noisered_filename=filtered_filename)
    snrdnf, wdnf = snr.calcSNRdnf()
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

    plt.figure("Cross correlation: inner vs outer")
    l,c = snr.calcCrossCorr()
    plt.xlim([-0.5,0.5])
    plt.xlabel("t/sec")
    plt.ylabel("V^2")
    plt.plot(l,c)

    plt.show()
