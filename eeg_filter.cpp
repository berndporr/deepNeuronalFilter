#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <boost/circular_buffer.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <Iir.h>
#include <Fir1.h>
#include <chrono>
#include <string>
#include <ctime>
#include <memory>
#include <numeric>
#include "cldl/Neuron.h"
#include "cldl/Layer.h"
#include "cldl/Net.h"
#include "parameters.h"
#include "dynamicPlots.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

using namespace std;
constexpr int ESC_key = 27;

int num_inputs = outerDelayLineLength;

// PLOT
#ifdef DoShowPlots
#define WINDOW "Deep Neuronal Filter"
const int plotW = 1200/2;
const int plotH = 720;
#endif

//NEURAL NETWORK
const int nNeurons[NLAYERS]={N10, N9, N8, N7, N6, N5, N4, N3, N2, N1, N0};
const int* numNeuronsP = nNeurons;
double w_eta = 0;
double b_eta = 0;

// GAINS
double outer_gain = 1;
double inner_gain = 1;
double remover_gain = 0;
double feedback_gain = 0;

void saveParam(fstream &params_file){
	params_file << "Gains: "    << "\n"
		    << outer_gain << "\n"
		    << inner_gain << "\n"
		    << remover_gain << "\n"
		    << feedback_gain << "\n"
		    << "Etas: " << "\n"
		    << w_eta << "\n"
		    << b_eta << "\n"
		    << "Network: " << "\n"
		    << NLAYERS << "\n"
		    << N10 << "\n"
		    << N9 << "\n"
		    << N8 << "\n"
		    << N7 << "\n"
		    << N6 << "\n"
		    << N5 << "\n"
		    << N4 << "\n"
		    << N3 << "\n"
		    << N2 << "\n"
		    << N1 << "\n"
		    << N0 << "\n"
		    << "LMS" << "\n"
		    << LMS_COEFF << "\n"
		    << LMS_LEARNING_RATE << "\n";
	params_file    << "didNoisePreFilter" << "\n"
		       << maxFilterLength << "\n";
	params_file    << "didSignalPreFilter" << "\n"
		       << maxFilterLength << "\n";
	params_file    << "didOuterDelayLine" << "\n"
		       << outerDelayLineLength << "\n";
	params_file    << "didSignalDelay" << "\n"
		       << innerDelayLineLength << "\n";
}


void processOneSubject(int subjIndex) {
	std::srand(1);

        //SIGNALS
	double inner_raw_data, outer_raw_data, p300trigger;

	boost::circular_buffer<double> oc_raw_buf(bufferLength);
	boost::circular_buffer<double> oc_buf(bufferLength);
	boost::circular_buffer<double> oc_end_buf(bufferLength);
	boost::circular_buffer<double> ic_buf(bufferLength);
	boost::circular_buffer<double> ic_raw_buf(bufferLength);
	boost::circular_buffer<double> oo_raw_buf(bufferLength);
	boost::circular_buffer<double> oo_buf(bufferLength);
	boost::circular_buffer<double> oo_end_buf(bufferLength);
	boost::circular_buffer<double> io_buf(bufferLength);
	boost::circular_buffer<double> io_raw_buf(bufferLength);
	boost::circular_buffer<double> ro_buf(bufferLength);
	boost::circular_buffer<double> rc_buf(bufferLength);
	boost::circular_buffer<double> f_nnc_buf(bufferLength);
	boost::circular_buffer<double> f_nno_buf(bufferLength);
	boost::circular_buffer<double> l1_c_buf(bufferLength);
	boost::circular_buffer<double> l2_c_buf(bufferLength);
	boost::circular_buffer<double> l3_c_buf(bufferLength);
	boost::circular_buffer<double> l1_o_buf(bufferLength);
	boost::circular_buffer<double> l2_o_buf(bufferLength);
	boost::circular_buffer<double> l3_o_buf(bufferLength);
//LMS
	boost::circular_buffer<double> lms_c_buf(bufferLength);
	boost::circular_buffer<double> lms_o_buf(bufferLength);
	
//adding delay line for the noise
	double outer_delayLine[outerDelayLineLength] ={0.0};
	boost::circular_buffer<double> innertrigger_delayLine(innerDelayLineLength);
	boost::circular_buffer<double> inner_delayLine(innerDelayLineLength);
	
// FILES
	fstream nn_file;
	fstream remover_file;
	fstream weight_file;
	fstream inner_file;
	fstream outer_file;
	fstream params_file;
	fstream lms_file;
	fstream lms_remover_file;
	fstream laplace_file;
	ifstream p300_infile;

long count = 0;
	//setting up the interactive window and the dynamic plot class
#ifdef DoShowPlots
	auto frame = cv::Mat(cv::Size(plotW, plotH), CV_8UC3);
	cvui::init(WINDOW, 1);
	dynaPlots plots(frame, plotW, plotH);
#endif
	//create files for saving the data and parameters
	string sbjct = std::to_string(subjIndex);
	Net NNO(NLAYERS, numNeuronsP, num_inputs, 0, "P300");
	nn_file.open(outpPrefix+"/subject" + sbjct + "/fnn.tsv", fstream::out);
	remover_file.open(outpPrefix+"/subject" + sbjct + "/remover.tsv", fstream::out);
	weight_file.open(outpPrefix+"/subject" + sbjct + "/lWeights.tsv", fstream::out);
	inner_file.open(outpPrefix+"/subject" + sbjct + "/inner.tsv", fstream::out);
	outer_file.open(outpPrefix+"/subject" + sbjct + "/outer.tsv", fstream::out);
	params_file.open(outpPrefix+"/subject" + sbjct + "/cppParams.tsv", fstream::out);
	lms_file.open(outpPrefix+"/subject" + sbjct + "/lmsOutput.tsv", fstream::out);
	lms_remover_file.open(outpPrefix+"/subject" + sbjct + "/lmsCorrelation.tsv", fstream::out);
	laplace_file.open(outpPrefix+"/subject" + sbjct + "/laplace.tsv", fstream::out);
	
	if (!params_file) {
		cout << "Unable to create files";
		exit(1); // terminate with error
	}
	
	char tmp[256];
	sprintf(tmp,"../noisewalls/EEG_recordings/participant%03d/rawp300.tsv",subjIndex);
	p300_infile.open(tmp);
	if (!p300_infile) {
		cout << "Unable to open file: " << tmp << endl;
		exit(1); // terminate with error
	}
	
	//setting up all the filters required
	Iir::Butterworth::HighPass<filterorder> outer_filterHP;
	outer_filterHP.setup(fs,highpassCutOff);
	Iir::Butterworth::BandStop<filterorder> outer_filterBS;
	outer_filterBS.setup(fs,powerlineFrequ,bsBandwidth);
	Iir::Butterworth::HighPass<filterorder> inner_filterHP;
	inner_filterHP.setup(fs,highpassCutOff);
	Iir::Butterworth::BandStop<filterorder> inner_filterBS;
	inner_filterBS.setup(fs,powerlineFrequ,bsBandwidth);
	
	Fir1 lms_filter(LMS_COEFF);
	lms_filter.setLearningRate(LMS_LEARNING_RATE);
	
	double corrLMS = 0;
	double lms_output = 0;

	//setting up the neural networks
	NNO.initNetwork(Neuron::W_RANDOM, Neuron::B_RANDOM, Neuron::Act_Sigmoid);

	// main loop processsing sample by sample
	while (!p300_infile.eof()) {
		count++;
		//get the data from .tsv files:
		p300_infile >> inner_raw_data >> outer_raw_data >> p300trigger;
		// GET ALL GAINS:
#ifdef DoShowPlots
		inner_gain = plots.get_inner_gain();
		outer_gain = plots.get_outer_gain();
		remover_gain = plots.get_remover_gain();
		feedback_gain = plots.get_feedback_gain();
#else
		inner_gain = 100;
		outer_gain = 100;
		remover_gain = 10;
		feedback_gain = 1;
#endif
		//A) INNER ELECTRODE:
		//1) ADJUST & AMPLIFY
		double inner_raw = inner_gain * inner_raw_data;
		double inner_filtered = inner_filterHP.filter(inner_raw);
		inner_filtered = inner_filterBS.filter(inner_filtered);

		//3) DELAY
		inner_delayLine.push_back(inner_filtered);
		double inner = inner_delayLine[0];
		
		innertrigger_delayLine.push_back(p300trigger);
		double delayedp300trigger = innertrigger_delayLine[0];

		//B) OUTER ELECTRODE:
		//1) ADJUST & AMPLIFY
		double outer_raw = outer_gain * outer_raw_data;
		double outer = outer_filterHP.filter(outer_raw);
		outer = outer_filterBS.filter(outer);

		//3) DELAY LINE
		for (int i = outerDelayLineLength-1 ; i > 0; i--) {
			outer_delayLine[i] = outer_delayLine[i-1];
			
		}
		outer_delayLine[0] = outer;
		double* outer_delayed = &outer_delayLine[0];
		
		// OUTER INPUT TO NETWORK
		NNO.setInputs(outer_delayed);
		NNO.propInputs();
		
		// REMOVER OUTPUT FROM NETWORK
		double remover = NNO.getOutput(0) * remover_gain;
		double f_nn = (inner - remover) * feedback_gain;
		
		// FEEDBACK TO THE NETWORK 
		NNO.setErrorCoeff(0, 1, 0, 0, 0, 0); //global, back, mid, forward, local, echo error
		NNO.setBackwardError(f_nn);
		NNO.propErrorBackward();
		
		// LEARN
#ifdef DoShowPlots
		w_eta = plots.get_wEta() / 10;
		b_eta = plots.get_bEta() / 10;
#else
		w_eta = 1;
		b_eta = 2;
#endif

		NNO.setLearningRate(w_eta, b_eta);
		if (count > maxFilterLength+outerDelayLineLength){
			NNO.updateWeights();
		}
		// SAVE WEIGHTS
		for (int i = 0; i < NLAYERS; i++) {
			weight_file << NNO.getLayerWeightDistance(i) << " ";
		}
		weight_file << NNO.getWeightDistance() << "\n";
		NNO.snapWeights(outpPrefix, "p300", subjIndex);
		double l1_o = NNO.getLayerWeightDistance(0);
		double l2_o = NNO.getLayerWeightDistance(1);
		double l3_o = NNO.getLayerWeightDistance(2);

		// Do Laplace filter
		double laplace = inner - outer;


		// Do LMS filter
		corrLMS += lms_filter.filter(outer);
		lms_output = inner - corrLMS;
		lms_filter.lms_update(lms_output);
		
		// SAVE SIGNALS INTO FILES
		laplace_file << laplace << "\t" << delayedp300trigger << endl;
		inner_file << inner << "\t" << delayedp300trigger << endl;
		remover_file << remover << endl;
		nn_file << f_nn << "\t" << delayedp300trigger << endl;
		lms_file << lms_output << "\t" << delayedp300trigger << endl;
		lms_remover_file << corrLMS << endl;
		
		// PUT VARIABLES IN BUFFERS
		// 1) MAIN SIGNALS
		oo_buf.push_back(outer_delayLine[0]);
		oo_end_buf.push_back(outer_delayLine[outerDelayLineLength-1]);
		oo_raw_buf.push_back(outer_raw);
		io_buf.push_back(inner);
		io_raw_buf.push_back(inner_raw);
		ro_buf.push_back(remover);
		f_nno_buf.push_back(f_nn);
		// 2) LAYER WEIGHTS
		l1_o_buf.push_back(l1_o);
		l2_o_buf.push_back(l2_o);
		l3_o_buf.push_back(l3_o);

		// 3) LMS outputs
		lms_o_buf.push_back(lms_output);
		
		// PUTTING BUFFERS IN VECTORS FOR PLOTS
		// 1) MAIN SIGNALS
		//      EXERCISE EYES
		std::vector<double> oo_plot(oo_buf.begin(), oo_buf.end());
		std::vector<double> oo_end_plot(oo_end_buf.begin(), oo_end_buf.end());
		std::vector<double> oo_raw_plot(oo_raw_buf.begin(), oo_raw_buf.end());
		std::vector<double> io_plot(io_buf.begin(), io_buf.end());
		std::vector<double> io_raw_plot(io_raw_buf.begin(), io_raw_buf.end());
		std::vector<double> ro_plot(ro_buf.begin(), ro_buf.end());
		std::vector<double> f_nno_plot(f_nno_buf.begin(), f_nno_buf.end());
		// 2) LAYER WEIGHTS
		std::vector<double> l1_o_plot(l1_o_buf.begin(), l1_o_buf.end());
		std::vector<double> l2_o_plot(l2_o_buf.begin(), l2_o_buf.end());
		std::vector<double> l3_o_plot(l3_o_buf.begin(), l3_o_buf.end());
		// LMS outputs
		std::vector<double> lms_o_plot(lms_o_buf.begin(), lms_o_buf.end());
		
#ifdef DoShowPlots
		frame = cv::Scalar(60, 60, 60);
		if (0 == (count % 10)) {
			plots.plotMainSignals(oo_raw_plot,
					       oo_plot,
					       oo_end_plot,
					       io_raw_plot,
					       io_plot,
					       ro_plot,
					       f_nno_plot,
					       l1_o_plot,
					       l2_o_plot,
					       l3_o_plot,
					       lms_o_plot, 1);
			plots.plotVariables();
			plots.plotTitle(sbjct, count, round(count / fs));
			cvui::update();
			cv::imshow(WINDOW, frame);

			if (cv::waitKey(1) == ESC_key) {
				break;
			}
		}
#endif
	}
	NNO.snapWeights(outpPrefix, "p300", subjIndex);
	saveParam(params_file);

	p300_infile.close();
	params_file.close();
	weight_file.close();
	remover_file.close();
	nn_file.close();
	inner_file.close();
	outer_file.close();
	lms_file.close();
	laplace_file.close();
	lms_remover_file.close();
	
	cout << "The program has reached the end of the input file" << endl;
}



int main(int argc, const char *argv[]) {
	if (argc < 2) {
		fprintf(stderr,"Usage: %s [-a] <subjectNumber>\n",argv[0]);
		fprintf(stderr,"       -a calculates all 20 subjects in a loop.\n");
		fprintf(stderr,"       Press ESC in the plot window to cancel the program.\n");
		return 0;
	}
	if (strcmp(argv[1],"-a") == 0) {
		for(int i = 0; i < nSubj; i++) {
			processOneSubject(i+1);
		}
		return 0;
	}
	const int subj = atoi(argv[1]);
	if ( (subj < 1) || (subj > nSubj) ) {
		fprintf(stderr,"Subj number of out range.\n");
		return -1;
	}
	processOneSubject(subj);
	return 0;
}
