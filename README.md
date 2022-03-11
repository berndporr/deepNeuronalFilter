# Deep Neuronal Filter (DNF)

## Prerequisites Libraries and packages:
1) Install the Fir1 library from _https://github.com/berndporr/fir1_ by running:

` sudo add-apt-repository ppa:berndporr/dsp `

` sudo apt-get install iir1` 

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

The folder `docs` contains the class ref.

## Applications

 - eeg_filter: removes noise from EEG (release)
 - ecg_filter: removes noise from ECG (alpha version)
