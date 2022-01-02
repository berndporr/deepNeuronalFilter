#pragma once


#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <ctgmath>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <fstream>
#include <iostream>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <numeric>
#include <vector>

#include "Neuron.h"

/**
 * This is the class for creating layers that are contained inside the Net class.
 * The Layer instances in turn contain neurons.
 */

class Layer {
public:
    /**
     * Constructor for Layer: it initialises the neurons internally.
     * @param _nNeurons Total number of neurons in the layer
     * @param _nInputs Total number of inputs to that layer
     */
    Layer(int _nNeurons, int _nInputs, int _subject, string _trial);
    /**
     * Destructor
     * De-allocated any memory
     **/
    ~Layer();

    /**
     * Options for what gradient of a chosen error to monitor
     */
    enum whichGradient {exploding = 0, average = 1, vanishing = 2};

    /**
     * Initialises each layer with specific methods for weight/bias initialisation and activation function of neurons
     * @param _layerIndex The index that is assigned to this layer by the Net class
     * @param _wim weights initialisation method,
     * see Neuron::weightInitMethod for different options
     * @param _bim biases initialisation method,
     * see Neuron::biasInitMethod for different options
     * @param _am activation method,
     * see Neuron::actMethod for different options
     */
    void initLayer(int _layerIndex, Neuron::weightInitMethod _wim, Neuron::biasInitMethod _bim, Neuron::actMethod _am);
    /** Sets the learning rate.
     * @param _learningRate Sets the learning rate for all neurons.
     **/
    void setlearningRate(double _w_learningRate, double _b_learningRate);
    /**
     * Sets the inputs to all neurons in the first hidden layer only
     * @param _inputs A pointer to an array of inputs
     */
	void setInputs(const double *_inputs, const double scale = 1.0, const unsigned int offset = 0, const int n = -1);
    /**
     * Sets the inputs to all neurons in the deeper layers (excluding the first hidden layer)
     * @param _index The index of the input
     * @param _value The value of the input
     */
    void propInputs(int _index, double _value);
    /**
     * Demands that all neurons in this layer calculate their output
     */
    void calcOutputs();

    /**
     * Sets the error to be propagated forward to all neurons in the first hidden layer only
     * @param _leadForwardError the error to be propagated forward
     */
    void setForwardError(double _leadForwardError);

    /**
     * Sets the error to be propagated forwards to all neurons in deeper layers
     * @param _index Index of input where the error originates form
     * @param _value The value of the error
     */
    void propErrorForward(int _index, double _value);

    /**
     * calculates the forward error by doing a weighed sum of forward errors and the weights
     */
    void calcForwardError();
    /**
     * Allows for accessing the forward error of a specific neuron.
     * @param _neuronIndex Index of the neuron to request the error from
     * @return Returns the forward error from the chosen neuron
     */
    double getForwardError(int _neuronIndex);

    /**
     * Sets the error to be propagated backward at all neurons in the output layer only.
     * @param _leadError the error to be propagated backward
     */
    void setBackwardError(double _leadError);
    /**
     * Sets the error to be propagated backward at all neurons, except those in the output layer.
     * @param _neuronIndex The index of the neuron receiving the weighted sum of errors
     * @param _nextSum The weighted sum of propagating error
     */
    void propErrorBackward(int _neuronIndex, double _nextSum);
    /**
     * Allows for accessing the error that propagates backward in the network
     * @param _neuronIndex The index from which the error is requested
     * @return Returns the error of the chosen neuron
     */
    double getBackwardError(int _neuronIndex);

    /**
     * Sets the middle error in all neurons in the chosen layer by Net
     * @param _leadMidError The error to be propagated bilaterally
     */
    void setMidError(double _leadMidError);
    /**
     * calculates the error to be propagated bilaterally
     */
    void calcMidError();

    /**
     * Allows for accessing the error that propagates bilaterally
     * @param _neuronIndex The index of the neuron that the error is requested from
     * @return Returns the mid error
     */
    double getMidError(int _neuronIndex);

    /**
     * Sets the mid error in all neurons of a chosen layer by Net
     * @param _index Index of the mid error
     * @param _value Value of the mid error
     */
    void propMidErrorForward(int _index, double _value);
    /**
     * Sets the mid error in all neurons of a specific layer chosen by Net
     * @param _neuronIndex The index of the neuron to receive the error
     * @param _nextSum The weighted sum of errors
     */
    void propMidErrorBackward(int _neuronIndex, double _nextSum);

    /**
     * It provides a measure of the magnitude of the error in this layer
     * to alarm for vanishing or exploding gradients.
     * \param _whichError choose what error to monitor, for more information see Neuron::whichError
     * \param _whichGradient choose what gradient of the chosen error to monitor,
     * for more information see Layer::whichGradient
     * @return Returns the chosen gradient in this layer
     */
    double getGradient(Neuron::whichError _whichError, whichGradient _whichGradient);

    /**
     * Sets the coefficient of the errors used for learning
     * @param _globalCoeff coefficient of the global error
     * @param _backwardsCoeff coefficient of the error propagating backward
     * @param _midCoeff coefficient of the error propagating bilaterally
     * @param _forwardCoeff coefficient of the error propagating forward
     * @param _localCoeff coefficient of the error propagating locally
     * @param _echoCoeff coefficient of the error resonating back and forth
     */
    void setErrorCoeff(double _globalCoeff, double _backwardsCoeff,
                        double _midCoeff, double _forwardCoeff,
                        double _localCoeff, double  _echoCoeff);
    /**
     * Requests that all neurons perform one iteration of learning
     */
    void updateWeights();

    /**
     * Sets the global error, all neurons will have access to this error
     * @param _globalError The global error
     */
    void setGlobalError(double _globalError);

    /**
     * Sets the local error at all neurons
     * @param _leadLocalError The error to be propagated locally only
     */
    void setLocalError(double _leadLocalError);

    /**
     * sets the error that propagates backwards and locally (for one layer only) for all neurons
     */
    void propGlobalErrorBackwardLocally(int _neuronIndex, double _nextSum);
    /**
     * Allows for accessing the local error of a specific neuron
     * @param _neuronIndex The index of the neuron to request the local error from
     * @return Returns the local error
     */
    double getLocalError(int _neuronIndex);

    /**
     * Sets the error to be resonated back and forth at all neurons
     * @param _echoError the resonating error
     */
    void setEchoError(double _clError);

    /**
     * Sets the resonating error for a specific neuron
     * @param _neuronIndex Index of the neurons receiving the error
     * @param _nextSum The weighted sum of propagating errors
     */
    void echoErrorBackward(int _neuronIndex, double _nextSum);

    /**
     * Allows for accessing the resonating error of a specific neuron
     * @param _neuronIndex The index of the neuron to reuquest the error form.
     * @return Returns the resonating error of the neuron
     */
    double getEchoError(int _neuronIndex);

    /**
     * Sets the resonating error for a specific neuron
     * @param _index the index of the incoming error
     * @param _value The value of the incoming error
     */
    void echoErrorForward(int _index, double _value);

    /**
     * Demands that all neurons calculate their resonating error
     */
    void calcEchoError();

    /**
     * Allows access to a specific neuron
     * @param _neuronIndex The index of the neuron to access
     * @return A pointer to that neuron
     */
    Neuron *getNeuron(int _neuronIndex);
    /**
     * Reports the number of neurons in this layer
     * @return The total number of neurons in this layer
     */
    int getnNeurons();
    /**
     * Allows for accessing the activation of a specific neuron
     * @param _neuronIndex The index of the neuron
     * @return the activation of that neuron
     */
    double getOutput(int _neuronIndex);
    /**
     * Allows for accessing the sum output of any specific neuron
     * @param _neuronIndex The index of the neuron to access
     * @return Returns the wighted sum of the inputs to that neuron
     */
    double getSumOutput(int _neuronIndex);
    /**
     * Allows for accessing any specific weights in the layer
     * @param _neuronIndex The index of the neuron containing that weight
     * @param _weightIndex The index of the input to which that weight is assigned
     * @return Returns the chosen weight
     */
    double getWeights(int _neuronIndex, int _weightIndex);
    /**
     * Accesses the total sum of weight changes of all the neurons in this layer
     * @return sum of weight changes all neurons
     */
    double getWeightChange();
    /**
     * Performs squared root on the weight change
     * @return The sqr of the weight changes
     */
    double getWeightDistance();
    /**
     * Reports the global error that is assigned to a specific neuron in this layer
     * @param _neuronIndex the neuron index
     * @return the value of the global error
     */
    double getGlobalError(int _neuronIndex);

    /**
     * Reports the initial value that was assigned to a specific weight at the initialisatin of the network
     * @param _neuronIndex Index of the neuron containing the weight
     * @param _weightIndex Index of the weight
     * @return
     */
    double getInitWeight(int _neuronIndex, int _weightIndex);

    /**
     * Saves the temporal weight change of all weights in all neurons into files
     */
    void saveWeights();
    /**
     * Snaps the final distribution of weights in a specific layer,
     * this is overwritten every time the function is called
     */
    void snapWeights(string prefix, string _trial, int _subject);
    void snapWeightsMatrixFormat(string prefix);
    /**
     * Prints on the console a full tree of this layer with the values of all weights and outputs for all neurons
     */
    void printLayer();

private:
    // initialisation:
    int nNeurons = 0;
    int nInputs = 0;
    double learningRate = 0;
    int myLayerIndex = 0;
    Neuron **neurons = 0;
    
    int layerHasReported = 0;

    //forward propagation of error:
    double leadForwardError = 0;

    //back propagation of error:
    double leadBackwardError = 0;

    //mid propagation of error:
    double leadMidError = 0;

    //global settings
    double globalError = 0;

    double leadLocalError =0;

    //exploding vasnishing gradient:
    double averageError = 0;
    double maxError = 0;
    double minError = 0;

    int subject = 0;
    string trial = "0";


    //learning:
    double weightChange=0;
};
