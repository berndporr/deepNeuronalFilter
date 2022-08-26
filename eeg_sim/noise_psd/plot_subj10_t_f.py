#!/usr/bin/python3
"""
Created on Fri Dec 24 18:28:41 2021
@author: luciamb, Bernd Porr 

Plots both channels of one subject & task
"""

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
        self.ch2 = self.data[:,8]
        if not filterData:
            return

        # Remove DC
        bHigh,aHigh = signal.butter(4,1/self.Fs*2,'high')
        self.ch1 = signal.lfilter(bHigh,aHigh,self.ch1);
        self.ch2 = signal.lfilter(bHigh,aHigh,self.ch2);
        
        # Remove 50Hz noise
        b50,a50 = signal.butter(4,[48/self.Fs*2,52/self.Fs*2],'stop')
        self.ch1 = signal.lfilter(b50,a50,self.ch1);
        self.ch2 = signal.lfilter(b50,a50,self.ch2);
        
        # Remove 150Hz interference
        b150,a150 = signal.butter(4,[148/self.Fs*2,152/self.Fs*2],'stop')
        self.ch1 = signal.lfilter(b150,a150,self.ch1);
        self.ch2 = signal.lfilter(b150,a150,self.ch2);

        # 2secs muted
        self.ch1[0:1000] = 0
        self.ch2[0:1000] = 0


# check if we run this as a main program
if __name__ == "__main__":
    task = tasks_eeg.TASKS[0]
    subj = 10
    toplot = tasks_eeg(subj,task)

    a = 0
    b = -1

    if len(sys.argv) < 2:
        print("Specify 'rest' or 'jawclench' to plot the spectrum at rest or during a jaw clench");
        quit()
    else:
        if sys.argv[1] == 'jawclench':
            a = int(toplot.Fs * 28.6)
            b = int(toplot.Fs * 30.6)
            
        if sys.argv[1] == 'rest':
            a = int(toplot.Fs * 18)
            b = int(toplot.Fs * 28)

    plt.figure('Time domain')
    plt.suptitle(task+', Participant '+str(subj))
    plt.subplot(211)
    plt.plot(toplot.t,toplot.ch1)
    plt.title('Channel 1')
    plt.xlabel('Time (sec)')
    plt.ylabel('Amplitude (V)')
    plt.subplot(212)
    plt.plot(toplot.t,toplot.ch2)
    plt.title('Channel 2')
    plt.xlabel('Time (sec)')
    plt.ylabel('Amplitude (V)')

    f1, Pxx_den1 = signal.periodogram(toplot.ch1[a:b],toplot.Fs,scaling='spectrum')
    f2, Pxx_den2 = signal.periodogram(toplot.ch2[a:b],toplot.Fs,scaling='spectrum')
    plt.figure("Freq domain")
    plt.suptitle(task+', Participant '+str(subj))
    plt.subplot(211)
    plt.plot(f1, np.log10(np.abs(Pxx_den1)))
    plt.title('Channel 1')
    plt.xlabel('Freq/Hz')
    plt.ylabel('Log Power (V^2/Hz)')
    plt.ylim([-16,-10])
    plt.subplot(212)
    plt.plot(f2, np.log10(np.abs(Pxx_den2)))
    plt.title('Channel 2')
    plt.xlabel('Freq/Hz')
    plt.ylabel('Log Power (V^2/Hz)')
    plt.ylim([-16,-10])
    plt.show()
