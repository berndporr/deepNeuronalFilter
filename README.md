# Deep Neuronal Filter (DNF)

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.6360675.svg)](https://doi.org/10.5281/zenodo.6360675)

## Prerequisites Libraries and packages

` sudo add-apt-repository ppa:berndporr/dsp `

` sudo apt-get install iir1-dev`

2) Install openCV library by running:

` sudo apt install libopencv-dev -y `

3) Install boost library by running:

` sudo apt-get install libboost-all-dev`

4) And make sure you have ` cmake ` installed.

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

 - eeg_filter: removes noise from EEG (release)
 - ecg_filter: removes noise from ECG (alpha version)
