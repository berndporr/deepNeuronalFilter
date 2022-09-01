#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
import sys
import getopt
from plotly.subplots import make_subplots
import plotly.graph_objects as go
from scipy import signal

class SimData:
    def loadFile(self,filename):
        if self.subj < 0:
            return np.loadtxt("../results/{}".format(filename))
        else:
            return np.loadtxt("../results/{}/{}".format(self.subj,filename))
    
    def __init__(self,subj,filtered_filename,startsec,endsec,useraw):
        self.fs = 500
        self.startsec = startsec
        self.subj = subj
        a = int(startsec * self.fs)
        if endsec:
            b = int(endsec * self.fs)
        else:
            b = int(-self.fs*2)
        dnfdata = self.loadFile(filtered_filename)
        self.dnf = dnfdata[a:b,0]
        self.remover = dnfdata[a:b,1]
        if useraw:
            self.inner = (self.loadFile("inner_raw.tsv"))[a:b]
            self.outer = (self.loadFile("outer_raw.tsv"))[a:b]
            print("Using raw input signals to DNF")
        else:
            self.inner = (self.loadFile("inner.tsv"))[a:b,0]
            self.outer = (self.loadFile("outer.tsv"))[a:b,0]
            print("using pre-filtered signalst to DNF")

    def getTimeAxis(self,data):
        return np.linspace(self.startsec,(len(data)/self.fs)+self.startsec,len(data))


def plotWithPlotly(subj,filtered_filename,startsec,endsec,useraw):
    simdata = SimData(subj,filtered_filename,startsec,endsec,useraw)

    fig = make_subplots(rows=4, cols=1,
                        shared_xaxes=True,
                        subplot_titles=("Inner ","Outer ", "Remover", "DNF out "),
                        vertical_spacing=0.1)

    fig.add_trace(go.Scatter(y=simdata.inner,x=simdata.getTimeAxis(simdata.inner)),
                  row=1, col=1)

    fig.add_trace(go.Scatter(y=simdata.outer,x=simdata.getTimeAxis(simdata.outer)),
                  row=2, col=1)
    
    fig.add_trace(go.Scatter(y=simdata.remover,x=simdata.getTimeAxis(simdata.dnf)),
                  row=3, col=1)
    
    fig.add_trace(go.Scatter(y=simdata.dnf,x=simdata.getTimeAxis(simdata.dnf)),
                  row=4, col=1)
    
    fig.update_layout(title_text="DNF timedomain explorer for subject {}".format(subj))
    fig.show()

    fig = make_subplots(rows=3, cols=1,
                        shared_xaxes=True,
                        subplot_titles=("Inner","Outer", "DNF out"),
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
    
    fig.update_layout(title_text="DNF frequency domain explorer for subject {}, {}".format(subj,filtered_filename))
    fig.show()


def plotWithMatplotlib(subj,filtered_filename,startsec,endsec,useraw):
    simdata = SimData(subj,filtered_filename,startsec,endsec,useraw)

    fig = plt.figure("DNF time domain explorer for experiment {}, {}".format(subj,filtered_filename))

    m = max(simdata.inner)
    if m < 0.0001:
        l = 0.0001
    elif m < 0.00025:
        l = 0.00025
    else:
        l = 0.0005
    
    ax = fig.add_subplot(4,1,1)
    ax.set_ylim([-l,l])
    ax.title.set_text('Inner')
    ax.plot(simdata.getTimeAxis(simdata.inner),simdata.inner)

    ax = fig.add_subplot(4,1,2)
    ax.set_ylim([-l,l])
    ax.title.set_text('Outer')
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
    
    fig = plt.figure("DNF frequ domain explorer for subject {}, {}".format(subj,filtered_filename))

    fmax = 2E-11
    
    ax = fig.add_subplot(4,1,1)
    ax.title.set_text('Inner')
    f, P = signal.periodogram(simdata.inner,simdata.fs)
    ax.set_xlim([0,100])
    # ax.set_ylim([0,fmax])
    ax.plot(f,P)

    ax = fig.add_subplot(4,1,2)
    ax.title.set_text('Outer')
    f, P = signal.periodogram(simdata.outer,simdata.fs)
    ax.set_xlim([0,100])
    # ax.set_ylim([0,fmax])
    ax.plot(f,P)

    ax = fig.add_subplot(4,1,3)
    ax.title.set_text('Remover')
    f, P = signal.periodogram(simdata.remover,simdata.fs)
    ax.set_xlim([0,100])
    # ax.set_ylim([0,fmax])
    ax.plot(f,P)

    ax = fig.add_subplot(4,1,4)
    ax.title.set_text('DNF output')
    f, P = signal.periodogram(simdata.dnf,simdata.fs)
    ax.set_xlim([0,100])
    # ax.set_ylim([0,fmax])
    ax.plot(f,P)
    
    fig.subplots_adjust(hspace=0.5)
    
    plt.show()


# check if we run this as a main program
if __name__ == "__main__":
    subj = -1
    startsec = 10
    endsec = False
    filtered_filename = "dnf.tsv"
    usePlotly = True
    useRaw = False

    helptext = 'usage: {} -p participant -s startsec -e endsec -f noiseredfile.tsv -m -h'.format(sys.argv[0])

    try:
        # Gather the arguments
        all_args = sys.argv[1:]
        opts, arg = getopt.getopt(all_args, 'p:s:e:f:t:mr')
        # Iterate over the options and values
        for opt, arg_val in opts:
            if '-p' in opt:
                subj = int(arg_val)
            elif 'r' in opt:
                useRaw = True
            elif '-s' in opt:
                startsec = float(arg_val)
            elif '-e' in opt:
                endsec = float(arg_val)
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
        plotWithPlotly(subj,filtered_filename,startsec,endsec,useRaw)
    else:
        plotWithMatplotlib(subj,filtered_filename,startsec,endsec,useRaw)
