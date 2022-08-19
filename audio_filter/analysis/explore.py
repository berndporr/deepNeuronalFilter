#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
import sys
import getopt
from plotly.subplots import make_subplots
import plotly.graph_objects as go
from scipy import signal

audiofolder = "audio"

class SimData:
    def loadFile(self,filename):
        fullpath = "../results/exp{}/{}".format(self.exper,filename)
        print("Loading:",fullpath)
        return np.loadtxt(fullpath,comments=';')
    
    def __init__(self,exper,filtered_filename,startsec,endsec=False):
        self.fs = 48000
        self.startsec = startsec
        self.exper = exper
        a = int(startsec * self.fs)
        if endsec:
            b = int(endsec * self.fs)
        else:
            b = int(-self.fs*2) 
        dnfdata = self.loadFile(filtered_filename+"_out.dat")
        self.dnf = dnfdata[a:b,1]
        removerdata = self.loadFile(filtered_filename+"_remover.dat")
        self.remover = removerdata[a:b]
        self.inner = (self.loadFile("signalWithNoise.dat"))[a:b,1]
        self.outer = (self.loadFile("noiseref.dat"))[a:b,1]

    def getTimeAxis(self,data):
        return np.linspace(self.startsec,(len(data)/self.fs)+self.startsec,len(data))


def plotWithPlotly(exper,filtered_filename,startsec,endsec):
    simdata = SimData(exper,filtered_filename,startsec,endsec)

    fig = make_subplots(rows=4, cols=1,
                        shared_xaxes=True,
                        subplot_titles=("Signal+Noise ","Noiseref ", "Remover", "DNF out "),
                        vertical_spacing=0.1)

    fig.add_trace(go.Scatter(y=simdata.inner,x=simdata.getTimeAxis(simdata.inner)),
                  row=1, col=1)

    fig.add_trace(go.Scatter(y=simdata.outer,x=simdata.getTimeAxis(simdata.outer)),
                  row=2, col=1)
    
    fig.add_trace(go.Scatter(y=simdata.remover,x=simdata.getTimeAxis(simdata.dnf)),
                  row=3, col=1)
    
    fig.add_trace(go.Scatter(y=simdata.dnf,x=simdata.getTimeAxis(simdata.dnf)),
                  row=4, col=1)
    
    fig.update_layout(title_text="DNF timedomain explorer for experiment {}".format(exper))
    fig.show()

    fig = make_subplots(rows=3, cols=1,
                        shared_xaxes=True,
                        subplot_titles=("Signal+Noise","Noiseref", "DNF out"),
                        vertical_spacing=0.1)

    f, Pxx_den = signal.periodogram(simdata.inner,simdata.fs)
    fig.add_trace(go.Scatter(y=Pxx_den, x=f),
                  row=1, col=1)

    f, Pxx_den = signal.periodogram(simdata.outer,simdata.fs)
    fig.add_trace(go.Scatter(y=Pxx_den, x=f),
                  row=2, col=1)

    f, Pxx_den = signal.periodogram(simdata.dnf,simdata.fs)
    fig.add_trace(go.Scatter(y=Pxx_den, x=f),
                  row=3, col=1)
    
    fig.update_layout(title_text="DNF frequency domain explorer for experiment {}, {}".format(exper,filtered_filename))
    fig.show()


def plotWithMatplotlib(exper,filtered_filename,startsec,endsec):
    simdata = SimData(exper,filtered_filename,startsec,endsec)

    fig = plt.figure("DNF time domain explorer for experiment {}, {}".format(exper,filtered_filename))

    m = max(simdata.inner)
    l = m
    
    ax = fig.add_subplot(4,1,1)
    ax.set_ylim([-l,l])
    ax.title.set_text('Signal+Noise')
    ax.plot(simdata.getTimeAxis(simdata.inner),simdata.inner)

    ax = fig.add_subplot(4,1,2)
    ax.set_ylim([-l,l])
    ax.title.set_text('Noiseref')
    ax.plot(simdata.getTimeAxis(simdata.outer),simdata.outer)

    ax = fig.add_subplot(4,1,3)
    ax.set_ylim([-l,l])
    ax.title.set_text('Remover')
    ax.plot(simdata.getTimeAxis(simdata.remover),simdata.remover)

    ax = fig.add_subplot(4,1,4)
    ax.set_ylim([-l,l])
    ax.title.set_text('DNF output')
    ax.plot(simdata.getTimeAxis(simdata.dnf),simdata.dnf)

    fig.subplots_adjust(hspace=0.5)
    
    fig = plt.figure("DNF frequ domain explorer for experiment {}, {}".format(exper,filtered_filename))
    
    ax = fig.add_subplot(4,1,1)
    ax.title.set_text('Signal+Noise')
    f, P = signal.periodogram(simdata.inner,simdata.fs)
    ax.semilogx(f,np.log10(P)*10)
    ax.set_xlim([20,20000])
    ax.set_ylim([-200,0])

    ax = fig.add_subplot(4,1,2)
    ax.title.set_text('Noiseref')
    f, P = signal.periodogram(simdata.outer,simdata.fs)
    ax.semilogx(f,np.log10(P)*10)
    ax.set_xlim([20,20000])
    ax.set_ylim([-200,0])

    ax = fig.add_subplot(4,1,3)
    ax.title.set_text('Remover')
    f, P = signal.periodogram(simdata.remover,simdata.fs)
    ax.semilogx(f,np.log10(P)*10)
    ax.set_xlim([20,20000])
    ax.set_ylim([-200,0])

    ax = fig.add_subplot(4,1,4)
    ax.title.set_text('DNF output')
    f, P = signal.periodogram(simdata.dnf,simdata.fs)
    ax.semilogx(f,np.log10(P)*10)
    ax.set_xlim([20,20000])
    ax.set_ylim([-200,0])
    
    fig.subplots_adjust(hspace=0.5)
    
    plt.show()


# check if we run this as a main program
if __name__ == "__main__":
    exper = 1
    startsec = 0.1
    endsec = False
    filtered_filename = "dnf"
    usePlotly = True

    helptext = 'usage: {} -p experimentNumber -s startsec -e endsec -f noiseredfile.dat -m -h'.format(sys.argv[0])

    try:
        # Gather the arguments
        all_args = sys.argv[1:]
        opts, arg = getopt.getopt(all_args, 'p:s:e:f:t:m')
        # Iterate over the options and values
        for opt, arg_val in opts:
            if '-p' in opt:
                exper = int(arg_val)
            elif '-s' in opt:
                startsec = int(arg_val)
            elif '-e' in opt:
                endsec = int(arg_val)
            elif '-f' in opt:
                filtered_filename = arg_val
            elif '-m' in opt:
                usePlotly = False
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        print ("Option -m switches over to matplotlib. Default is plotly.")
        sys.exit(2)

    if usePlotly:
        plotWithPlotly(exper,filtered_filename,startsec,endsec)
    else:
        plotWithMatplotlib(exper,filtered_filename,startsec,endsec)
