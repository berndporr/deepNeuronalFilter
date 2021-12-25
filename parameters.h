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
const double innerHighpassCutOff = 0.5; // Hz
const double outerHighpassCutOff = 10; // Hz
const double powerlineFrequ = 50; // Hz
const double bsBandwidth = 2.5; // Hz

const int samplesNoLearning = 3 * fs / innerHighpassCutOff;

// file path prefix for the results
const std::string outpPrefix = "results";

//creat circular buffers for plotting
const int bufferLength = 1000 ;

#define LMS_LEARNING_RATE 0.0001

#define DoShowPlots

const int outerDelayLineLength = fs / outerHighpassCutOff * 2;
const int innerDelayLineLength = outerDelayLineLength / 2;

//NN specifications
#define DoDeepLearning

const int nNeurons[]={29, 17, 11, 5, 3, 2, 1};

#endif //EEGFILTER_PARAMETERS_H
