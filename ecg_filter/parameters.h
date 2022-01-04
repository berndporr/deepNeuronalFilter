//
// Created by sama on 25/06/19.
//
#ifndef ECGFILTER_PARAMETERS_H
#define ECGFILTER_PARAMETERS_H

// number of subjects
const int nSubj = 20;

// pre-filtering
const int filterorder = 2;
const double ecgemgHighpassCutOff = 0.5; // Hz
const double emgnoiseHighpassCutOff = 2.5; // Hz

const double powerlineFrequ = 50; // Hz
const double bsBandwidth = 2.5; // Hz

//creat circular buffers for plotting
const int bufferLength = 1000 ;

// learning rate
const double w_eta = 50;

const int NLAYERS = 6;

#define LMS_LEARNING_RATE 0

const double ecgemg_gain = 100;
const double emgnoise_gain = 100;

const double remover_gain = 1;

// Very slow
// #define SAVE_WEIGHTS

#endif //ECGFILTER_PARAMETERS_H
