//
// Created by sama on 25/06/19.
//
#ifndef EEGFILTER_PARAMETERS_H
#define EEGFILTER_PARAMETERS_H

// number of subjects
const int nSubj = 20;

// sampling rate
const float fs = 250;

const int filterorder = 4;
const double highpassCutOff = 2; // Hz
const double powerlineFrequ = 50; // Hz
const double bsBandwidth = 2.5; // Hz

// file path prefix for the results
const std::string outpPrefix = "results";

//creat circular buffers for plotting
const int bufferLength = 1000 ;

#define LMS_COEFF (int)(fs)
#define LMS_LEARNING_RATE 0.00001

#define DoShowPlots

#define maxFilterLength (fs)

// NOISE:
#define doOuterPreFilter
// #define doOuterDelayLine
#define outerDelayLineLength 59

// SIGNAL:
#define doInnerPreFilter
// #define doInnerDelay
#define innerDelayLineLength 59

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
