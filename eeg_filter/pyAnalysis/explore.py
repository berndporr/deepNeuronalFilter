#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
import sys
import getopt
from plotly.subplots import make_subplots
import plotly.graph_objects as go
from scipy import signal

p300folder = "p300"

class SimData:
    def loadFile(self,filename):
        return np.loadtxt("../{}/subject{}/{}".format(self.task,self.subj,filename))
    
    def __init__(self,task,subj,filtered_filename,startsec):
        self.task = task
        if p300folder in task:
            self.fs = 250
        else:
            self.fs = 500
        self.startsec = startsec
        self.subj = subj
        a = startsec * self.fs
        dnfdata = self.loadFile(filtered_filename)
        self.dnf = dnfdata[a:-self.fs*2,0]
        self.remover = dnfdata[a:-self.fs*2,1]
        self.inner = (self.loadFile("inner.tsv"))[a:-self.fs*2,0]
        self.outer = (self.loadFile("outer.tsv"))[a:-self.fs*2,0]

    def getTimeAxis(self,data):
        return np.linspace(0,len(data)/self.fs,len(data))


def plotWithPlotly(task,subj,filtered_filename,startsec):
    simdata = SimData(task,subj,filtered_filename,startsec)

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
    
    fig.update_layout(title_text="DNF frequency domain explorer for subject {}".format(subj))
    fig.show()


def plotWithMatplotlib(task,subj,filtered_filename,startsec):
    simdata = SimData(task,subj,filtered_filename,startsec)

    fig = plt.figure()
    
    ax = fig.add_subplot(4,1,1)
    ax.set_ylim([-0.0005,0.0005])
    ax.title.set_text('Inner')
    ax.plot(simdata.getTimeAxis(simdata.inner),simdata.inner)

    ax = fig.add_subplot(4,1,2)
    ax.set_ylim([-0.0005,0.0005])
    ax.title.set_text('Outer')
    ax.plot(simdata.getTimeAxis(simdata.outer),simdata.outer)

    ax = fig.add_subplot(4,1,3)
    ax.set_ylim([-0.0005,0.0005])
    ax.title.set_text('Remover')
    ax.plot(simdata.getTimeAxis(simdata.remover),simdata.remover)

    ax = fig.add_subplot(4,1,4)
    ax.set_ylim([-0.0005,0.0005])
    ax.title.set_text('DNF output')
    ax.plot(simdata.getTimeAxis(simdata.dnf),simdata.dnf)
    
    plt.show()


# check if we run this as a main program
if __name__ == "__main__":
    subj = 10
    startsec = 60
    task = p300folder
    filtered_filename = "dnf.tsv"
    usePlotly = True

    helptext = 'usage: {} -p participant -s startsec -f file -n task -h'.format(sys.argv[0])

    try:
        # Gather the arguments
        all_args = sys.argv[1:]
        opts, arg = getopt.getopt(all_args, 'p:s:f:n:m')
        # Iterate over the options and values
        for opt, arg_val in opts:
            if '-p' in opt:
                subj = int(arg_val)
            elif '-s' in opt:
                startsec = int(arg_val)
            elif '-f' in opt:
                filtered_filename = arg_val
            elif '-n' in opt:
                task = arg_val
            elif '-m' in opt:
                usePlotly = False
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        sys.exit(2)

    if usePlotly:
        plotWithPlotly(task,subj,filtered_filename,startsec)
    else:
        plotWithMatplotlib(task,subj,filtered_filename,startsec)
