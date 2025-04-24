#ifndef ULTRASOUNDFILTER_PARAMETERS_H
#define ULTRASOUNDFILTER_PARAMETERS_H

#include "dnf.h"

const Neuron::actMethod ACTIVATION = Neuron::Act_Tanh;

const double fs = 2.5E9; // Hz

const double startTime = 1.5E-5; // seconds

const char datavarname[] = "data";

const std::string outpPrefix = "results";

// pre-filtering
const int filterorder = 4;
const double highpassCutOff = 8E6; // Hz
const double lowpassCutOff = 8E6; // Hz

//create circular buffers for plotting
const int bufferLength = 1000 ;

// dnf learning rate
const double dnf_learning_rate = 0.05;

// dnf number of layers
const int NLAYERS = 6;

// LMS learning rates
const double lms_learning_rate = 0.001;

const double remover_gain = 1;

const int nTapsDNF = fs / highpassCutOff;

const double transmitter1freq = 4E6;
const double transmitter2freq = 6E6;
const double bandwidth = 1E6;

// Very slow
// #define SAVE_WEIGHTS

#endif //EEGFILTER_PARAMETERS_H
