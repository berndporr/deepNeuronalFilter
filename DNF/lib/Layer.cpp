#include "cldl/Layer.h"
#include "cldl/Neuron.h"

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
    neurons = new Neuron*[nNeurons];
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

void Layer::setInputs(const double* _inputs){
    /*this is only for the first layer*/
    inputs=_inputs;
    for (int j=0; j<nInputs; j++){
        Neuron** neuronsp = neurons;//point to the 1st neuron
        /* sets a temporarily pointer to neuron-pointers
         * within the scope of this function. this is inside
         * the loop, so that it is set to the first neuron
         * everytime a new value is distributed to neurons */
        double input= *inputs; //take this input value
        for (int i=0; i<nNeurons; i++){
            (*neuronsp)->setInput(j,input);
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
//forward propagation of error:
//*************************************************************************************

void Layer::setForwardError(double _leadForwardError){
    /*this is only for the first layer*/
    leadForwardError=_leadForwardError;
    for (int i=0; i<nNeurons; i++){
        neurons[i]->setForwardError(leadForwardError);
    }
}

void Layer::propErrorForward(int _index, double _value){
    for (int i=0; i<nNeurons; i++){
        neurons[i]->propErrorForward(_index, _value);
    }
}

void Layer::calcForwardError(){
    for (int i=0; i<nNeurons; i++){
        neurons[i]->calcForwardError();
    }
}

double Layer::getForwardError(int _neuronIndex){
    return (neurons[_neuronIndex]->getForwardError());
}

//*************************************************************************************
//back propagation of error:
//*************************************************************************************

void Layer::setBackwardError(double _leadBackwardError){
    /* this is only for the final layer */
    leadBackwardError = _leadBackwardError;
    for (int i=0; i<nNeurons; i++){
        neurons[i]->setBackwardError(leadBackwardError);
    }
}

void Layer::propErrorBackward(int _neuronIndex, double _nextSum){
    neurons[_neuronIndex]->propErrorBackward(_nextSum);
    // if (_neuronIndex == 0){
    //   cout << " BP>> acc2=Sum(W*E): " << _nextSum;
    //   cout << " e=acc*sigmoid'(acc1): " << neurons[_neuronIndex]->getError();
    //   cout << " FP>> acc1=Sum(w.in): " << neurons[_neuronIndex]->getSumOutput();
    //   cout << " sigmoid(sum): " << neurons[_neuronIndex]->getOutput();
    //   cout << " " << endl;
    // }
}

double Layer::getBackwardError(int _neuronIndex){
    return (neurons[_neuronIndex]->getBackwardError());
}

//*************************************************************************************
//mid propagation of error:
//*************************************************************************************

void Layer::setMidError(double _leadMidError){
    /* this is only for the final layer */
    leadMidError = _leadMidError;
    for (int i=0; i<nNeurons; i++){
        neurons[i]->setMidError(leadMidError);
    }
}

void Layer::calcMidError(){
    for (int i=0; i<nNeurons; i++){
        neurons[i]->calcMidError();
    }
}

double Layer::getMidError(int _neuronIndex){
    return (neurons[_neuronIndex]->getMidError());
}

void Layer::propMidErrorForward(int _index, double _value){
    for (int i=0; i<nNeurons; i++){
        neurons[i]->propMidErrorForward(_index, _value);
    }
}

void Layer::propMidErrorBackward(int _neuronIndex, double _nextSum){
    neurons[_neuronIndex]->propMidErrorBackward(_nextSum);
}

//*************************************************************************************
//exploding/vanishing gradient:
//*************************************************************************************

double Layer::getGradient(Neuron::whichError _whichError, whichGradient _whichGradient) {
    averageError = 0;
    maxError = -100;
    minError = 100;
    switch(_whichGradient){
        case exploding:
            for (int i=0; i<nNeurons; i++){
                maxError = max(maxError, neurons[i]->getError(_whichError));
            }
            return maxError;
            break;
        case average:
            for (int i=0; i<nNeurons; i++){
                averageError += neurons[i]->getError(_whichError);
            }
            return averageError/nNeurons;
            break;
        case vanishing:
            for (int i=0; i<nNeurons; i++){
                minError = min(minError, neurons[i]->getError(_whichError));
            }
            return minError;
            break;
    }
    return 0;
}

//*************************************************************************************
//learning:
//*************************************************************************************

void Layer::setErrorCoeff(double _globalCoeff, double _backwardsCoeff, double _midCoeff, double _forwardCoeff, double _localCoeff, double  _echoCoeff){
    for (int i=0; i<nNeurons; i++){
        neurons[i]->setErrorCoeff(_globalCoeff, _backwardsCoeff, _midCoeff, _forwardCoeff, _localCoeff, _echoCoeff);
    }
}

void Layer::updateWeights(){
    for (int i=0; i<nNeurons; i++){
        neurons[i]->updateWeights();
    }
}

//*************************************************************************************
//global settings
//*************************************************************************************

void Layer::setGlobalError(double _globalError){
    globalError = _globalError;
    for (int i=0; i<nNeurons; i++){
        neurons[i]->setGlobalError(globalError);
    }
}

void Layer::setEchoError(double _echoError) {
    /* this is only for the final layer */
    for (int i=0; i<nNeurons; i++){
        neurons[i]->setEchoError(_echoError);
    }
}

double Layer::getEchoError(int _neuronIndex){
    return (neurons[_neuronIndex]->getEchoError());
}

void Layer::echoErrorBackward(int _neuronIndex, double _nextSum){
    neurons[_neuronIndex]->echoErrorBackward(_nextSum);
}

void Layer::echoErrorForward(int _index, double _value){
    for (int i=0; i<nNeurons; i++){
        neurons[i]->echoErrorForward(_index, _value);
    }
}

void Layer::calcEchoError(){
    for (int i=0; i<nNeurons; i++){
        neurons[i]->calcEchoError();
    }
}

//*************************************************************************************
//local backpropagation of error
//*************************************************************************************

void Layer::setLocalError(double _leadLocalError){
    /* this is only for the final layer */
    leadLocalError = _leadLocalError;
    for (int i=0; i<nNeurons; i++){
        neurons[i]->setLocalError(leadLocalError);
    }
}

void Layer::propGlobalErrorBackwardLocally(int _neuronIndex, double _nextSum){
    neurons[_neuronIndex]->propGlobalErrorBackwardLocally(_nextSum);
}

double Layer::getLocalError(int _neuronIndex){
    return (neurons[_neuronIndex]->getLocalError());
}

//*************************************************************************************
//getters:
//*************************************************************************************

Neuron* Layer::getNeuron(int _neuronIndex){
    assert(_neuronIndex < nNeurons);
    return (neurons[_neuronIndex]);
}

double Layer::getGlobalError(int _neuronIndex){
    return (neurons[_neuronIndex]->getGlobalError());
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

void Layer::snapWeights(string _where, string _trial, int _subject){
    subject = _subject;
    trial = _trial;
    std::fstream wfile;
    string name = "./cppData/subject" + std::to_string(subject) + "/grayLayer" + std::to_string(myLayerIndex+1) + "_" + trial + "_subject" + std::to_string(subject) + ".csv";
    wfile.open(name, fstream::out);
    if (!wfile || !wfile) {
        cout << "Unable to open grayScale files";
        exit(1); // terminate with error
    }
    for (int i=0; i<nNeurons; i++){
        for (int j=0; j<nInputs; j++){
            wfile << neurons[i]->getGRAYWeights(j) << " ";
        }
        wfile << "\n";
    }
    wfile.close();
}

void Layer::snapWeightsMatrixFormat(){
    std::ofstream wfile;
    char l = '0';
    l += myLayerIndex + 1;
    string name = "../cppOutputFiles/subject" + std::to_string(subject) + "/greyScaleMATRIX_Layer" + l
                  + "_Subject" + std::to_string(subject)
                  + "_" + trial;
    //name += l;
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
