#ifndef ECGFILTER_PARAMETERS_H
#define ECGFILTER_PARAMETERS_H

#include "dnf.h"

const char infilename[] = "ecg50hz.dat";
const char outfilename[] = "ecg_filtered.dat";

const float fs = 1000;

double noise_f = 50; //Hz 

const int nTapsDNF = 200;

const Neuron::actMethod ACTIVATION = Neuron::Act_Tanh;

// dnf learning rate
const double dnf_learning_rate = 0.05;

// dnf number of layers
const int NLAYERS = 5;

#endif //EEGFILTER_PARAMETERS_H
