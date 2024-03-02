# Deep Neuronal Filter (DNF)

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7100537.svg)](https://doi.org/10.5281/zenodo.7100537)

## Prerequisites Libraries and packages

1) Install the IIR and FIR filter libraries

Installation instructions are in these repositories:

 - IIR: https://github.com/berndporr/iir1
 - FIR: https://github.com/berndporr/fir1

2) Install openCV library by running:

Linux: `sudo apt install libopencv-dev`, Mac: `brew install opencv`

3) Install boost library by running:

Linux: `sudo apt-get install libboost-dev`, Mac: is included in opencv

4) And make sure you have `cmake` installed.

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
 - ecg_filter: removes EMG noise from ECG (alpha version)
 - audio_filter: removes noise from audio (alpha version)
