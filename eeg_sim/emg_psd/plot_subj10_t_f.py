#!/usr/bin/python3
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as signal
import sys
from scipy import signal

class tasks_eeg:
    TASKS = ["jawclench", "read", "colour", "wordsearch", "sudoku", "phoneApp", "lyingEC", "lyingEO"]
    Fs = 500
    
    def __init__(self,_participant,_task,filterData=True):
        '''
        _participant is the participant number
        _task is one of the tasks from the TASKS array
        filterData if set to true the EEG data is filtered:
        Highpass filter at 1Hz to remove Dc.
        Notch filter betwwen 48 and 52Hz to remove 50Hz noise.
        Bandstop filter betwwen 148 and 152Hz to remove 150Hz interference.
        
        Butterworth filter used. Show flat frequency response in the passband.
        '''
        self.participant = _participant
        self.task = _task
        fullpath = "../../../gla_researchdata_1258/EEG_recordings/participant{:03d}/{}.tsv".format(self.participant,self.task)
        self.data = np.loadtxt(fullpath)
        
        self.t = np.linspace(0,len(self.data)/self.Fs,len(self.data))
        self.ch1 = self.data[:,7]

        # Remove DC
        bHigh,aHigh = signal.butter(4,0.2/self.Fs*2,'high')
        self.ch1 = signal.lfilter(bHigh,aHigh,self.ch1);
        
        # Remove 50Hz noise
        b50,a50 = signal.butter(4,[49/self.Fs*2,51/self.Fs*2],'stop')
        self.ch1 = signal.lfilter(b50,a50,self.ch1);
        
        # Remove 150Hz interference
        b150,a150 = signal.butter(4,[148/self.Fs*2,152/self.Fs*2],'stop')
        self.ch1 = signal.lfilter(b150,a150,self.ch1);

        self.ch1 = self.ch1[self.Fs*20:]


# check if we run this as a main program
if __name__ == "__main__":
    task = tasks_eeg.TASKS[0]
    subj = 10
    toplot = tasks_eeg(subj,task)

    jawclenchIntervals = [
        [ 9.7, 10.6 ],
        [ 25.0, 25.9 ],
        [ 40.0, 40.5 ],
        [ 55.2, 55.9 ],
        [ 69.7, 70.6 ],
        [ 85.0, 86.0 ],
        [ 99.7, 100.6 ]
    ]

    ch1 = toplot.ch1

    a = 0
    b = -1
    title = "Full EEG recording during repetitve jaw clench"

    ch1 = np.empty(0)
    for i in jawclenchIntervals:
        a = int(toplot.Fs * i[0])
        b = int(toplot.Fs * i[1])
        print("Using jaw clench: {}sec - {}sec".format(i[0],i[1]))
        ch1 = np.append(ch1,toplot.ch1[a:b])
    title = "Jawclench segments"

    t = np.linspace(0,len(ch1)/toplot.Fs,len(ch1))

    plt.figure()
    plt.suptitle('Participant '+str(subj)+' '+title)
    plt.subplot(211)
    plt.plot(t,ch1 * 1E6)
    plt.xlabel('Time (sec)')
    plt.ylabel('Amplitude (uV)')
    plt.ylim([-400,400])
    plt.subplot(212)

    f, Pxx_den = signal.periodogram(ch1,toplot.Fs,scaling='spectrum')
    plt.semilogy(f, np.abs(Pxx_den))
    plt.xlabel('Freq/Hz')
    plt.ylabel('Log Power (V^2/Hz)')
    plt.ylim([1E-12,1E-10])
    plt.xlim([0,100])

    fc = 50
    bw = 70
    b,a = signal.butter(2,[(fc-bw/2)/toplot.Fs*2,(fc+bw/2)/toplot.Fs*2],'band')
    w, h = signal.freqz(b,a)
    plt.semilogy(w / (2*np.pi) * toplot.Fs, np.abs(h*5E-12)) # square absorbed
    
    plt.show()
