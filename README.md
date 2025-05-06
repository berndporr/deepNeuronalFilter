# Deep Neuronal Filter (DNF)

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7100537.svg)](https://doi.org/10.5281/zenodo.7100537)

## Prerequisites Libraries and packages

1) Install the IIR and FIR filter libraries

Installation instructions are in these repositories:

 - IIR: https://github.com/berndporr/iir1
 - FIR: https://github.com/berndporr/fir1

2) Install the boost library by running:

```
sudo apt-get install libboost-dev
```

3) Make sure you have `cmake` installed.

4) Optionally, install the openCV library by running:

```
sudo apt install libopencv-dev
```

OpenCV is used for realtime plotting of the EEG filter but not needed for the DNF library itself.


## How to compile

Type:

```
cmake .
```
to create the makefile and then

```
make
```
to compile the library and the demos.

## Installation

```
sudo make install
```

## Documentation

 - Online: https://berndporr.github.io/deepNeuronalFilter/
 - PDF: https://github.com/berndporr/deepNeuronalFilter/blob/main/docs/pdf/refman.pdf

## Applications

 - eeg_filter: removes noise from real EEG data (release)
 - eeg_sim: removes noise from simulated EEG data (release)

both need openCV to be installed as they use it for realtime plotting of the filtering process.
