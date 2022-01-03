#ifndef _DNF_H
#define _DNF_H

#include "dnf/Neuron.h"
#include "dnf/Layer.h"
#include "dnf/Net.h"

class DNF {
public:
DNF(const int NLAYERS, const int numTaps, double fs) : noiseDelayLineLength(numTaps),
		signalDelayLineLength(noiseDelayLineLength / 2),
		signal_delayLine(signalDelayLineLength),
		nNeurons(new int[NLAYERS]),
		noise_delayLine(new double[noiseDelayLineLength]) {

	// calc an exp reduction of the numbers always reaching 1
	double b = exp(log(noiseDelayLineLength)/(NLAYERS-1));
	for(int i=0;i<NLAYERS;i++) {
		nNeurons[i] = noiseDelayLineLength / pow(b,i);
		if (i == (NLAYERS-1)) nNeurons[i] = 1;
		fprintf(stderr,"Layer %d has %d neurons.\n",i,nNeurons[i]);
	}
	
	//create the neural network
	NNO = new Net(NLAYERS, nNeurons, noiseDelayLineLength * 2, 0, "");
	
	//setting up the neural networks
	NNO->initNetwork(Neuron::W_RANDOM, Neuron::B_NONE, Neuron::Act_NONE);
}

	double filter(double signal, double noise) {
		signal_delayLine.push_back(signal);
		const double delayed_signal = signal_delayLine[0];
		
		for (int i = noiseDelayLineLength-1 ; i > 0; i--) {
			noise_delayLine[i] = noise_delayLine[i-1];
		}
		noise_delayLine[0] = noise / (double)noiseDelayLineLength;

		// NOISE INPUT TO NETWORK
		NNO->setInputs(noise_delayLine, 1,0,noiseDelayLineLength);
		NNO->setInputs(noise_delayLine,-1,noiseDelayLineLength,noiseDelayLineLength);
		NNO->propInputs();
		
		// REMOVER OUTPUT FROM NETWORK
		remover = NNO->getOutput(0);
		f_nn = delayed_signal - remover;
		
		// FEEDBACK TO THE NETWORK 
		NNO->setError(f_nn);
		NNO->propErrorBackward();
		NNO->updateWeights();
		return f_nn;
	}

	inline Net& getNet() const {
		return *NNO;
	}

	inline const int getSignalDelaySteps() const {
		return signalDelayLineLength;
	}

	inline const double getDelayedSignal() const {
		return signal_delayLine[0];
	}

	inline const double getRemover() const {
		return remover;
	}

	inline const double getOutput() const {
		return f_nn;
	}

	~DNF() {
		delete NNO;
		delete[] nNeurons;
		delete[] noise_delayLine;
	}

private:
	Net *NNO;
	int noiseDelayLineLength;
	int signalDelayLineLength;
	boost::circular_buffer<double> signal_delayLine;
	double* noise_delayLine;
	int* nNeurons;
	double remover = 0;
	double f_nn = 0;
};

#endif
