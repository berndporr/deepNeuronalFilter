import matplotlib.pyplot as plt
import numpy as np
import sys
import getopt
from plotly.subplots import make_subplots
import plotly.graph_objects as go

class SimData:
    def loadFile(self,filename):
        return np.loadtxt("../results/subject{}/{}".format(self.subj,filename))
    
    def __init__(self,subj,startsec=0,fs=250):
        self.fs = fs
        self.startsec = startsec
        self.subj = subj
        a = startsec * fs
        self.fnn = (self.loadFile("fnn.tsv"))[a:-fs,0]
        self.inner = (self.loadFile("inner.tsv"))[a:-fs,0]
        self.outer = (self.loadFile("outer.tsv"))[a:-fs,0]
        
# check if we run this as a main program
if __name__ == "__main__":
    subj = 1
    startsec = 120

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
                        vertical_spacing=0.02)

    fig.add_trace(go.Scatter(y = simdata.inner),
                  row=1, col=1)

    fig.add_trace(go.Scatter(y = simdata.outer),
                  row=2, col=1)
    
    fig.add_trace(go.Scatter(y = simdata.fnn),
                  row=3, col=1)
    
    fig.update_layout(title_text="DNF overview")
    fig.show()
