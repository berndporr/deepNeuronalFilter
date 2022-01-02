//
// Created by sama on 25/06/19.
//
#ifndef EEGFILTER_PARAMETERS_H
#define EEGFILTER_PARAMETERS_H

// number of subjects
const int nSubj = 20;

// pre-filtering
const int filterorder = 2;
const double innerHighpassCutOff = 0.5; // Hz

const double outerHighpassCutOff = 1; // Hz

const double powerlineFrequ = 50; // Hz
const double bsBandwidth = 2.5; // Hz

//creat circular buffers for plotting
const int bufferLength = 1000 ;

// learning rate
const double w_eta = 5;

const int NLAYERS = 6;

#define LMS_LEARNING_RATE 0.005

const double inner_gain = 1000;
const double outer_gain = 1000;
const double remover_gain = 0.25;

#define DoShowPlots

// Very slow
// #define SAVE_WEIGHTS

#endif //EEGFILTER_PARAMETERS_H
