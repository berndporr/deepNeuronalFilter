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
    freq, power = signal.welch(y,fs,scaling="spectrum",nperseg=fs)
    w = np.array([])
    for f,p in zip(freq, power):
        if (f >= 5) and (f < 50):
            s = s + p
            if not w.any():
                w = np.array([f,p])
            else:
                w = np.row_stack((w,np.array([f,p])))
    return s,w

def calcSNRinner(subj,startsec=10,fs=250,folder="results"):
    filename = "inner.tsv"
    NoisePwr,w = calcNoisePower(subj,filename,0,startsec,fs,folder=folder)
    vep = p300.calcVEP(subj,filename,startsec,fs)
    SignalPwr = np.mean(vep[int(fs*0.4):]**2)
    print("Signal Power:",SignalPwr)
    print("NoisePwr:",NoisePwr)
    snr = np.log10(SignalPwr/NoisePwr)*10
    return snr,w

def calcSNRdnf(subj,filename,startsec=10,fs=250,folder="results"):
    NoisePwr,w = calcNoisePower(subj,filename,0,startsec,fs,folder=folder)
    vep = p300.calcVEP(subj,filename,startsec,fs)
    SignalPwr = np.mean(vep[int(fs*0.4):]**2)
    print("Signal Power:",SignalPwr)
    print("NoisePwr:",NoisePwr)
    snr = np.log10(SignalPwr/NoisePwr)*10
    return snr,w

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

    if subj == 0:
        finner2dnf = open("inner2dnf.dat","wt")
        for subj in [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]:
            print("Subject",subj)
            snrdnf, wdnf = calcSNRdnf(subj,filtered_filename,startsec=startsec,folder=noisefolder)
            snrinner, winner = calcSNRinner(subj,startsec=startsec,folder=noisefolder)
            impr = snrdnf-snrinner
            print("SNR improvement: {} - {} = {}".format(snrinner,snrdnf,impr))
            finner2dnf.write(str(impr)+"\n")
        finner2dnf.close()
        sys.exit(0)

    snr, wdnf = calcSNRdnf(subj,filtered_filename,startsec=startsec,folder=noisefolder)
    print("SNR from Noise removal:",snr)
    plt.plot(wdnf[:,0],wdnf[:,1],label="DNF")
    plt.legend()
    print()
    print()
    snr, winner = calcSNRinner(subj,startsec=startsec,folder=noisefolder)
    print("SNR just from inner:",snr)
    plt.plot(winner[:,0],winner[:,1],label="INNER")
    plt.legend()

    plt.show()
