import matplotlib.pyplot as plt
import numpy as np
import sys
import getopt
from plotly.subplots import make_subplots
import plotly.graph_objects as go
from scipy import signal

class SimData:
    def loadFile(self,filename):
        return np.loadtxt("../results/subject{}/{}".format(self.subj,filename))
    
    def __init__(self,subj,startsec=0,fs=250):
        self.fs = fs
        self.startsec = startsec
        self.subj = subj
        a = startsec * fs
        self.dnf = (self.loadFile("dnf.tsv"))[a:-fs,0]
        self.inner = (self.loadFile("inner.tsv"))[a:-fs,0]
        self.outer = (self.loadFile("outer.tsv"))[a:-fs,0]

    def getTimeAxis(self,data):
        return np.linspace(0,len(data)/fs,len(data))

# check if we run this as a main program
if __name__ == "__main__":
    subj = 1
    startsec = 120
    fs = 250

    helptext = 'usage: {} -p participant -s startsec -f file -h'.format(sys.argv[0])

    try:
        # Gather the arguments
        all_args = sys.argv[1:]
        opts, arg = getopt.getopt(all_args, 'p:s:f:')
        # Iterate over the options and values
        for opt, arg_val in opts:
            if '-p' in opt:
                subj = int(arg_val)
            elif '-s' in opt:
                startsec = int(arg_val)
            elif '-f' in opt:
                filtered_filename = arg_val
            elif '-h' in opt:
                raise getopt.GetoptError()
            else:
                raise getopt.GetoptError()
    except getopt.GetoptError:
        print (helptext)
        sys.exit(2)

    simdata = SimData(subj,startsec=startsec)

    fig = make_subplots(rows=3, cols=1,
                        shared_xaxes=True,
                        subplot_titles=("Inner ","Outer ", "DNF out "),
                        vertical_spacing=0.1)

    fig.add_trace(go.Scatter(y=simdata.inner,x=simdata.getTimeAxis(simdata.inner)),
                  row=1, col=1)

    fig.add_trace(go.Scatter(y=simdata.outer,x=simdata.getTimeAxis(simdata.outer)),
                  row=2, col=1)
    
    fig.add_trace(go.Scatter(y=simdata.dnf,x=simdata.getTimeAxis(simdata.dnf)),
                  row=3, col=1)
    
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
