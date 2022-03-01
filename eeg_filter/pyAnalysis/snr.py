import matplotlib.pyplot as plt
import numpy as np
import sys
import p300
import scipy.signal as signal
import getopt


class SNR:
    def __init__(self,subj=1,startsec=10,fs=250,folder="results"):
        self.subj = subj
        self.startsec = startsec
        self.fs = fs
        self.folder = folder
        self.fnn_filename = "fnn.tsv"
        self.inner_filename = "inner.tsv"
    
    def calcNoisePower(self,filename):
        p = "../{}/subject{}/{}".format(self.folder,self.subj,filename)
        d = np.loadtxt(p)
        ll = self.fs * self.startsec
        y = d[ll:,0]
        s = 0
        freq, power = signal.welch(y,self.fs,scaling="spectrum",nperseg=fs)
        w = np.array([])
        for f,p in zip(freq, power):
            if (f >= 5) and (f < 50):
                s = s + p
                if not w.any():
                    w = np.array([f,p])
                else:
                    w = np.row_stack((w,np.array([f,p])))
        return s,w

    def calcSNRinner(self):
        NoisePwr,w = self.calcNoisePower(self.inner_filename)
        vep = p300.calcVEP(subj,self.inner_filename,self.startsec,self.fs)
        SignalPwr = np.mean(vep[int(self.fs*0.4):]**2)
        print("Signal Power:",SignalPwr)
        print("NoisePwr:",NoisePwr)
        snr = np.log10(SignalPwr/NoisePwr)*10
        return snr,w

    def calcSNRdnf(self):
        NoisePwr,w = self.calcNoisePower(self.fnn_filename)
        vep = p300.calcVEP(self.subj,self.fnn_filename,self.startsec,self.fs)
        SignalPwr = np.mean(vep[int(self.fs*0.4):]**2)
        print("Signal Power:",SignalPwr)
        print("NoisePwr:",NoisePwr)
        snr = np.log10(SignalPwr/NoisePwr)*10
        return snr,w

# check if we run this as a main program
if __name__ == "__main__":
    subj = 1
    startsec = 120
    noisefolder = "results"
    fs = 250

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
                fs = 500
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        sys.exit(2)

    if subj == 0:
        finner2dnf = open("inner2dnf.dat","wt")
        for subj in [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]:
            print("Subject",subj)
            snr = SNR(subj=subj,startsec=startsec,fs=fs,folder=noisefolder)
            snrdnf, wdnf = snr.calcSNRdnf()
            snrinner, winner = snr.calcSNRinner()
            impr = snrdnf-snrinner
            print("SNR improvement: {} - {} = {}".format(snrinner,snrdnf,impr))
            finner2dnf.write(str(impr)+"\n")
        finner2dnf.close()
        sys.exit(0)

    snr = SNR(subj=subj,startsec=startsec,fs=fs,folder=noisefolder)
    snrdnf, wdnf = snr.calcSNRdnf()
    print("SNR from Noise removal:",snrdnf)
    plt.plot(wdnf[:,0],wdnf[:,1],label="DNF")
    plt.legend()
    print()
    print()
    snrinner, winner = snr.calcSNRinner()
    print("SNR just from inner:",snrinner)
    plt.plot(winner[:,0],winner[:,1],label="INNER")
    plt.legend()

    plt.show()
