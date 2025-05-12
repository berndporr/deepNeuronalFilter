/**
 * Filter demo which removes 50Hz from an ECG containing
 * 50Hz noise.
 **/

#include <chrono>
#include <iostream>
#include <stdio.h>
#include <thread>

#define _USE_MATH_DEFINES
#include <math.h>

#include "dnf.h"


using namespace std; 

// Integer of the total number of hidden layers
// required not including the input layer
const int NLAYERS = 5;

// Number of taps of the delay line
const int nTapsDNF = 100;

// Sampling rate
double fs = 1000; // Hz

// Mains noise
double noise_f = 50; //Hz

// activation
const Neuron::actMethod ACTIVATION = Neuron::Act_Tanh;

// dnf learning rate
const double dnf_learning_rate = 0.1;

// input filename
const char inputFilename[] = "ecg50hz.dat";

// output filename
const char outputFilename[] = "ecg_filtered.dat";

int main(int argc, char* argv[]){
    fprintf(stderr, "Reading noisy ECG file: %s.\n",inputFilename);

    FILE *finput = fopen(inputFilename,"rt");
    FILE *foutput = fopen(outputFilename,"wt");

    double norm_noise_f = noise_f / fs;
    int nSamples = 0;

    DNF dnf(NLAYERS,nTapsDNF,fs,ACTIVATION);

    auto start = std::chrono::high_resolution_clock::now();

    for(int i=0;;i++) 
	{
	    //reading the input signal and generating the ref_noise
	    double input_signal;		
	    if (fscanf(finput,"%lf\n",&input_signal)<1) break;
	    nSamples++;
	    // scale it to -/+1
	    input_signal -= 2048.0;
	    input_signal /= 2048.0;

	    double ref_noise = sin(2*M_PI*norm_noise_f*(double)i);

	    if (i > nTapsDNF){
		dnf.getNet().setLearningRate(dnf_learning_rate, 0);
	    } else {
		dnf.getNet().setLearningRate(0, 0);
	    }

	    double f_nn = dnf.filter(input_signal, ref_noise);

	    fprintf(foutput,"%f %f %f\n",f_nn, input_signal, ref_noise);
	}

    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    const auto seconds_taken = (double)(std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count())/1E6;
    const double maxSamplingRate = nSamples / seconds_taken;

    printf("Time taken = %f s, max sampling rate = %f Hz\n", seconds_taken, maxSamplingRate);

    fprintf(stderr, "Written result to: %s.\n",outputFilename);

    fclose(finput);
    fclose(foutput);
}
