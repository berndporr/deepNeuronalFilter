# Data analysis tools

All tools are python commandline tools which have the same parameters:
 - -p participant number between 1 and 20
 - -s The second from where data analysis should happen (usually after learning)
 - -t The task which is either "p300" or "jawclench2.
 - -f The filter output file used. That can be either "dnf.tsv" or "lms.tsv"
 - -h Help

## explore.py

Displays a timedomain and frequency domain plot either in
matplotlib or in the browser.
```
./explore.py -p participant -s startsec -e endsec -f noiseredfile.tsv -t task -m -h
```
 - Option -m switches over to matplotlib. Default is plotly.
 - Option -e is the second where the plot ends in the timedomain.

## p300.py
Plots the p300 if a subject.
```
./p300.py -p participant -s startsec -f file -h
```

## snr.py
Calculates the SNR of one subject
```
./p300.py -p participant -s startsec -f file -h
```

## SNRcompareDNFLMS.py
Compares DNF SNR increase against LMS SNR increase for jawclench.
Usually no parameters are needed here.
