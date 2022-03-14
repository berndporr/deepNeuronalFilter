#ifndef EEGFILTER_PARAMETERS_H
#define EEGFILTER_PARAMETERS_H

#include "dnf.h"

const char* const p300Path = "../../gla_researchdata_1258/EEG_recordings/participant%03d/rawp300.tsv";
const char* const tasksPath = "../../gla_researchdata_1258/EEG_recordings/participant%03d/%s.tsv";

const Neuron::actMethod ACTIVATION = Neuron::Act_Tanh;

// number of subjects
const int nSubj = 20;

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
const double dnf_learning_rate_p300 = 10;
const double dnf_learning_rate_tasks = 2.5;

// dnf number of layers
const int NLAYERS = 6;

// LMS learning rates
const double lms_learning_rate_p300 = 0.04;
const double lms_learning_rate_tasks = 0.01;

const double inner_gain = 1000;
const double outer_gain = 1000;
const double remover_gain = 1;

// Very slow
// #define SAVE_WEIGHTS

#endif //EEGFILTER_PARAMETERS_H
