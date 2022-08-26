#ifndef EEGFILTER_PARAMETERS_H
#define EEGFILTER_PARAMETERS_H

#include "dnf.h"

const int nExperiments = 20;
const float duration = 120; // sec
const float alpha = 0.4;

const float averageNoiseAmplitude = 15; // uV (pre filtering)
const float standardDevNoiseAmplitude = 5; // uV (for stats)

const double noiseModelBandpassCenter = 50; // Hz
const double noiseModelBandpassWidth = 70; // Hz
const double jawclenchEverySec = 15; // sec
const double jawclenchDuration = 1.5; // sec
const double jawclenchNoiseBoost = 5; // gain

const float signalAmplitude = 35; // uV (before lowpass filtering)
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
