#ifndef AUDIO_FILTER_PARAMETERS_H
#define AUDIO_FILTER_PARAMETERS_H

#include "dnf.h"

const double fs = 48000;

const char* const audioPath = "audio/exp%d/signal_noise.wav";

const Neuron::actMethod ACTIVATION = Neuron::Act_Tanh;

// number of experiments
const int nExp = 2;

// pre-filtering
const int filterorder = 2;
const double signalWithNoiseHighpassCutOff = 20; // Hz
const double noiserefHighpassCutOff = 500; // Hz

//creat circular buffers for plotting
const int bufferLength = 1000 ;

// Twice the number of taps.
const int nTapsDNF = int(fs / noiserefHighpassCutOff) * 3;

// dnf learning rate
const double dnf_learning_rate = 1;

// dnf number of layers
const int NLAYERS = 15;

// LMS learning rates
const double lms_learning_rate = 0.05;

const double signalWithNoise_gain = 1;
const double noiseref_gain = 1;
const double remover_gain = 1;

// PLOTTING
#define WINDOW "Deep Neuronal Filter"
const int plotW = 1200;
const int plotH = 720;


#endif //AUDIO_FILTER_PARAMETERS_H
