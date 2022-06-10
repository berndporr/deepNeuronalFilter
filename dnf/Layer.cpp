#include "Layer.h"
#include "Neuron.h"

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
#include <fstream>

//*************************************************************************************
// constructor de-constructor
//*************************************************************************************

Layer::Layer(int _nNeurons, int _nInputs, int _subject, string _trial){
    subject = _subject;
    trial = _trial;
    nNeurons = _nNeurons; // number of neurons in this layer
    nInputs = _nInputs; // number of inputs to each neuron
    neurons = new Neuron*[(unsigned)nNeurons];
    /* dynamic allocation of memory to n number of
     * neuron-pointers and returning a pointer, "neurons",
     * to the first element */
    for (int i=0;i<nNeurons;i++){
        neurons[i]=new Neuron(nInputs);
    }
    /* each element of "neurons" pointer is itself a pointer
     * to a neuron object with specific no. of inputs*/
     //cout << "layer" << endl;
}

Layer::~Layer(){
    for(int i=0;i<nNeurons;i++) {
        delete neurons[i];
    }
    delete[] neurons;
    //delete[] inputs;
    /* it is important to delete any dynamic
     * memory allocation created by "new" */
}

//*************************************************************************************
//initialisation:
//*************************************************************************************

void Layer::initLayer(int _layerIndex, Neuron::weightInitMethod _wim, Neuron::biasInitMethod _bim, Neuron::actMethod _am){
    myLayerIndex = _layerIndex;
    for (int i=0; i<nNeurons; i++){
        neurons[i]->initNeuron(i, myLayerIndex, _wim, _bim, _am);
    }
}

void Layer::setlearningRate(double _w_learningRate, double _b_learningRate){
    for (int i=0; i<nNeurons; i++){
        neurons[i]->setLearningRate(_w_learningRate,_b_learningRate);
    }
}

//*************************************************************************************
//forward propagation of inputs:
//*************************************************************************************

void Layer::setInputs(const double* const _inputs, const double scale, const unsigned int offset, const int n) {
	/*this is only for the first layer*/
	const double* inputs = _inputs;
	for (int j=0; j< (n < 0 ? nInputs:n); j++){
		Neuron** neuronsp = neurons;//point to the 1st neuron
		/* sets a temporarily pointer to neuron-pointers
		 * within the scope of this function. this is inside
		 * the loop, so that it is set to the first neuron
		 * everytime a new value is distributed to neurons */
		const double input= (*inputs) * scale; //take this input value
		for (int i=0; i<nNeurons; i++){
		  (*neuronsp)->setInput(j+(int)offset,input);
			//set this input value for this neuron
			neuronsp++; //point to the next neuron
		}
		inputs++; //point to the next input value
	}
}

void Layer::propInputs(int _index, double _value){
	for (int i=0; i<nNeurons; i++){
		neurons[i]->propInputs(_index, _value);
	}
}

void Layer::calcOutputs(){
	for (int i=0; i<nNeurons; i++){
		layerHasReported = neurons[i]->calcOutput(layerHasReported);
	}
}

//*************************************************************************************
//back propagation of error:
//*************************************************************************************

void Layer::setError(double _backwardError){
    /* this is only for the final layer */
    backwardError = _backwardError;
    for (int i=0; i<nNeurons; i++){
        neurons[i]->setError(backwardError);
    }
}

//*************************************************************************************
//exploding/vanishing gradient:
//*************************************************************************************

double Layer::getGradient(whichGradient _whichGradient) {
    averageError = 0;
    maxError = -100;
    minError = 100;
    switch(_whichGradient){
        case exploding:
            for (int i=0; i<nNeurons; i++){
                maxError = max(maxError, neurons[i]->getError());
            }
            return maxError;
            break;
        case average:
            for (int i=0; i<nNeurons; i++){
                averageError += neurons[i]->getError();
            }
            return averageError/nNeurons;
            break;
        case vanishing:
            for (int i=0; i<nNeurons; i++){
                minError = min(minError, neurons[i]->getError());
            }
            return minError;
            break;
    }
    return 0;
}

//*************************************************************************************
//learning:
//*************************************************************************************

void Layer::updateWeights(){
    for (int i=0; i<nNeurons; i++){
        neurons[i]->updateWeights();
    }
}

//*************************************************************************************
//getters:
//*************************************************************************************

Neuron* Layer::getNeuron(int _neuronIndex){
    assert(_neuronIndex < nNeurons);
    return (neurons[_neuronIndex]);
}

double Layer::getSumOutput(int _neuronIndex){
    return (neurons[_neuronIndex]->getSumOutput());
}

double Layer::getWeights(int _neuronIndex, int _weightIndex){
    return (neurons[_neuronIndex]->getWeights(_weightIndex));
}

double Layer::getInitWeight(int _neuronIndex, int _weightIndex){
    return (neurons[_neuronIndex]->getInitWeights(_weightIndex));
}

double Layer::getWeightChange(){
    weightChange=0;
    for (int i=0; i<nNeurons; i++){
        weightChange += neurons[i]->getWeightChange();
    }
    //cout<< "Layer: WeightChange is: " << weightChange << endl;
    return (weightChange);
}

double Layer::getWeightDistance(){
    return sqrt(weightChange);
}

double Layer::getOutput(int _neuronIndex){
    return (neurons[_neuronIndex]->getOutput());
}

int Layer::getnNeurons(){
    return (nNeurons);
}

//*************************************************************************************
//saving and inspecting
//*************************************************************************************

void Layer::saveWeights(){
    for (int i=0; i<nNeurons; i++){
        neurons[i]->saveWeights();
    }
}

void Layer::snapWeights(string prefix, string _trial, int _subject){
    subject = _subject;
    trial = _trial;
    std::fstream wfile;
    string name = prefix+"/subject" + std::to_string(subject) + "/grayLayer" + std::to_string(myLayerIndex+1) + "_" + trial + "_weights.csv";
    wfile.open(name, fstream::out);
    if (!wfile || !wfile) {
        cout << "Unable to open grayScale files";
        exit(1); // terminate with error
    }
    for (int i=0; i<nNeurons; i++){
        for (int j=0; j<nInputs; j++){
            wfile << neurons[i]->getWeights(j) << " ";
        }
        wfile << "\n";
    }
    wfile.close();
}

void Layer::snapWeightsMatrixFormat(string prefix){
    std::ofstream wfile;
    string name = prefix+"/subject" + std::to_string(subject) + "/MATRIX_Layer"
                  + "_Subject" + std::to_string(subject)
                  + "_" + trial;
    name += ".csv";
    wfile.open(name);
    wfile << "[" << nNeurons << "," << nInputs << "]";
    wfile << "(";
    for (int i=0; i<nNeurons; i++){
        if (i == 0){
            wfile << "(";
        }else{
            wfile << ",(";
        }
        for (int j=0; j<nInputs; j++){
            if (j == 0){
                wfile << neurons[i]->getWeights(j);
            }else{
                wfile << "," << neurons[i]->getWeights(j);
            }
        }
        wfile << ")";
        //wfile << "\n";
    }
    wfile << ")";
    wfile.close();
}

void Layer::printLayer(){
    cout<< "\t This layer has " << nNeurons << " Neurons" <<endl;
    cout<< "\t There are " << nInputs << " inputs to this layer" <<endl;
    for (int i=0; i<nNeurons; i++){
        cout<< "\t Neuron number " << i << ":" <<endl;
        neurons[i]->printNeuron();
    }

}
