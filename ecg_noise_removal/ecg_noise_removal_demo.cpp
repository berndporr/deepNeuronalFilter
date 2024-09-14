
#include <chrono>

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <boost/circular_buffer.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/lexical_cast.hpp>
#include <Iir.h>
#include <chrono>
#include <string>
#include <ctime>
#include <thread>         // std::thread
#include <memory>
#include <numeric>
#include "dnf.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "parameters.h"

using namespace std; 

int main(int argc, char* argv[]){

    FILE *finput = fopen(infilename,"rt");
    FILE *foutput = fopen(outfilename,"wt");
    
    DNF dnf(NLAYERS,nTapsDNF,fs,ACTIVATION);

    dnf.getNet().setLearningRate(dnf_learning_rate, 0);

    auto start = std::chrono::high_resolution_clock::now();

    double norm_noise_f = noise_f / fs;

    int nSamples;
    for(nSamples=0;;nSamples++) 
	{
	    double input_signal;		
	    if (fscanf(finput,"%lf\n",&input_signal)<1) break;
	    input_signal -= 2048.0;
	    input_signal /= 2048.0;

	    double ref_noise = sin(2 * M_PI * norm_noise_f * (double)nSamples);

        double f_nn = dnf.filter(input_signal,ref_noise);

	    fprintf(foutput,"%f %f %f\n",f_nn, input_signal, dnf.getRemover());
	}

    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    const auto seconds_taken = (double)(std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count())/1E6;

    const double maxSamplingRate = nSamples / seconds_taken;

    printf("Time taken = %f s, max sampling rate = %f Hz\n", seconds_taken, maxSamplingRate);

    fclose(finput);
    fclose(foutput);

    fprintf(stderr,"Written the filtered ECG to '%s'\n",outfilename);
}
