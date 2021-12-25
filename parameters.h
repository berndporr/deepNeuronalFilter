//
// Created by sama on 25/06/19.
//
#ifndef EEGFILTER_PARAMETERS_H
#define EEGFILTER_PARAMETERS_H

// number of subjects
const int nSubj = 20;

// sampling rate
const int fs = 250;

// pre-filtering
const int filterorder = 4;
const double highpassCutOff = 0.5; // Hz
const double powerlineFrequ = 50; // Hz
const double bsBandwidth = 2.5; // Hz

const int samplesNoLearning = 2 * fs / highpassCutOff;

// file path prefix for the results
const std::string outpPrefix = "results";

//creat circular buffers for plotting
const int bufferLength = 1000 ;

#define LMS_LEARNING_RATE 1

#define DoShowPlots

#define outerDelayLineLength (fs)

#define innerDelayLineLength (fs/2)

//NN specifications
#define DoDeepLearning
#define NLAYERS 11
#define N10 29 //10
#define N9 23 //9
#define N8 19 //8
#define N7 17 //7
#define N6 13 //6
#define N5 11 //5
#define N4 7 //4
#define N3 5 //3
#define N2 3 //2
#define N1 2 //1
#define N0 1 //this has to always be 1

#endif //EEGFILTER_PARAMETERS_H
