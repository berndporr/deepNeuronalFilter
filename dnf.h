#ifndef _DNF_H
#define _DNF_H

#include <boost/circular_buffer.hpp>

#include "dnf/Neuron.h"
#include "dnf/Layer.h"
#include "dnf/Net.h"

/**
 * Main Deep Neuronal Network main class.
 * It's designed to be as simple as possible with
 * only a few parameters as possible.
 **/
class DNF {
public:
	/**
	 * Constructor which sets up the delay lines, network layers
	 * and also calculates the number of neurons per layer so
	 * that the final layer always just has one neuron.
	 * \param NLAYERS Number of layers
	 * \param numTaps Number of taps for the delay line feeding into the 1st layer
	 * \param fs Sampling rate of the signals used in Hz.
	 * \param am The activation function for the neurons. Default is tanh.
	 **/
	DNF(const int NLAYERS,
	    const int numTaps,
	    const double fs,
	    const Neuron::actMethod am = Neuron::Act_Tanh,
	    const bool debugOutput = false
		) : noiseDelayLineLength(numTaps),
			 signalDelayLineLength(noiseDelayLineLength / 2),
			 signal_delayLine(signalDelayLineLength),
			 nNeurons(new int[NLAYERS]),
			 noise_delayLine(new double[noiseDelayLineLength]) {		
		// calc an exp reduction of the numbers always reaching 1
		double b = exp(log(noiseDelayLineLength)/(NLAYERS-1));
		for(int i=0;i<NLAYERS;i++) {
			nNeurons[i] = ceil(noiseDelayLineLength / pow(b,i));
			if (i == (NLAYERS-1)) nNeurons[i] = 1;
		}
		
		//create the neural network
		NNO = new Net(NLAYERS, nNeurons, noiseDelayLineLength, 0, "");
		
		//setting up the neural networks
		for(int i=0;i<NLAYERS;i++) {
			NNO->getLayer(i)->initLayer(i,Neuron::W_RANDOM, Neuron::B_NONE, am);
			if (debugOutput) {
				fprintf(stderr,"Layer %d has %d neurons. act = %d\n",i,nNeurons[i],am);
			}
		}
	}

	enum ErrorPropagation { Backprop = 0, ModulatedHebb = 1 };

	void setErrorPropagation(const ErrorPropagation e) {
		errorPropagation = e;
	}

	/**
	 * Realtime sample by sample filtering operation
	 * \param signal The signal contaminated with noise. Should be less than one.
	 * \param noise The reference noise. Should be less than one.
	 * \returns The filtered signal where the noise has been removed by the DNF.
	 **/
	double filter(const double signal, const double noise) {
		signal_delayLine.push_back(signal);
		const double delayed_signal = signal_delayLine[0];
		
		for (int i = noiseDelayLineLength-1 ; i > 0; i--) {
			noise_delayLine[i] = noise_delayLine[i-1];
		}
		noise_delayLine[0] = noise / (double)noiseDelayLineLength;

		// NOISE INPUT TO NETWORK
		NNO->setInputs(noise_delayLine);
		NNO->propInputs();
		
		// REMOVER OUTPUT FROM NETWORK
		remover = NNO->getOutput(0);
		f_nn = delayed_signal - remover;
		
		// FEEDBACK TO THE NETWORK 
		NNO->setError(f_nn);
		switch (errorPropagation) {
		case Backprop:
		default:
			NNO->propErrorBackward();
			break;
		case ModulatedHebb:
			NNO->propModulatedHebb(f_nn);
			break;
		}
		NNO->updateWeights();
		return f_nn;
	}

	/**
	 * Returns a reference to the whole neural network.
	 * \returns A reference to Net.
	 **/
	inline Net& getNet() const {
		return *NNO;
	}

	/**
	 * Returns the length of the delay line which
	 * delays the signal polluted with noise.
	 * \returns Number of delay steps in samples.
	 **/
	inline const int getSignalDelaySteps() const {
		return signalDelayLineLength;
	}

	/**
	 * Returns the delayed with noise polluted signal by the delay 
	 * indicated by getSignalDelaySteps().
	 * \returns The delayed noise polluted signal sample.
	 **/
	inline const double getDelayedSignal() const {
		return signal_delayLine[0];
	}

	/**
	 * Returns the remover signal.
	 * \returns The current remover signal sample.
	 **/
	inline const double getRemover() const {
		return remover;
	}

	/**
	 * Returns the output of the DNF: the the noise
	 * free signal.
	 * \returns The current output of the DNF which is idential to filter().
	 **/
	inline const double getOutput() const {
		return f_nn;
	}

	/**
	 * Frees the memory used by the DNF.
	 **/
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
	ErrorPropagation errorPropagation = Backprop;
};

#endif
