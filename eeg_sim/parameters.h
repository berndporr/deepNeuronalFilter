#ifndef EEGFILTER_PARAMETERS_H
#define EEGFILTER_PARAMETERS_H

#include "dnf.h"

const double noiseModelBandpassCenter = 50; // Hz
const double noiseModelBandpassWidth = 30; // Hz
const double signalModelLowpassFreq = 17; // Hz

const Neuron::actMethod ACTIVATION = Neuron::Act_Tanh;

// pre-filtering
const int filterorder = 2;
const double innerHighpassCutOff = 0.5; // Hz
const double outerHighpassCutOff = 5; // Hz
const double LaplaceCutOff = 0.5; // Hz

const double powerlineFrequ = 50; // Hz
const double bsBandwidth = 2.5; // Hz

//creat circular buffers for plotting
const int bufferLength = 1000 ;

// dnf learning rate
const double dnf_learning_rate = 2.5;

// dnf number of layers
const int NLAYERS = 6;

// LMS learning rates
const double lms_learning_rate = 0.01;

const double inner_gain = 1000;
const double outer_gain = 1000;
const double remover_gain = 1;

#endif //EEGFILTER_PARAMETERS_H
