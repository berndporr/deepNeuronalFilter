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
int startTime = time(NULL);

//creat circular buffers for plotting
const int bufferLength = 1000 ;
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
#ifdef DoDeepLearning
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
#endif
//LMS
boost::circular_buffer<double> lms_c_buf(bufferLength);
boost::circular_buffer<double> lms_o_buf(bufferLength);

//adding delay line for the noise
double outer_p300_delayLine[outerDelayLineLength] ={0.0};
boost::circular_buffer<double> inner_p300trigger_delayLine(innerDelayLineLength);
boost::circular_buffer<double> inner_p300_delayLine(innerDelayLineLength);

// CONSTANTS
//const float fs = 250;
const int num_subjects = 12;

#ifdef doOuterDelayLine
int num_inputs = outerDelayLineLength;
#else
int num_inputs = 1;
#endif

// PLOT
#ifdef DoShowPlots
#define WINDOW "data & stat frame"
dynaPlots* plots;
int plotW = 1200;
int plotH = 720;
#endif

//NEURAL NETWORK
#ifdef DoDeepLearning
int nNeurons[NLAYERS]={N10, N9, N8, N7, N6, N5, N4, N3, N2, N1, N0};
int* numNeuronsP = nNeurons;
Net* NNO = new Net(NLAYERS, numNeuronsP, num_inputs, 0, "P300");
double w_eta_p300 = 0;
double b_eta_p300 = 0;
#endif

//FILTERS
Fir1 *outer_filter;
Fir1 *inner_filter;
Fir1 *lms_filter_exercise = nullptr;
Fir1 *lms_filter_p300 = nullptr;


//SIGNALS
double inner_p300_raw_data, outer_p300_raw_data, p300trigger;

// GAINS
double outer_exercise_gain = 1;
double outer_p300_gain = 1;
double inner_exercise_gain = 1;
double inner_p300_gain = 1;
#ifdef DoDeepLearning
double remover_exercise_gain = 0;
double remover_p300_gain = 0;
double feedback_exercise_gain = 0;
double feedback_p300_gain = 0;
#endif
// FILES
#ifdef DoDeepLearning
fstream nn_file;
fstream remover_file;
fstream weight_exercise_file;
fstream weight_p300_file;
#endif
fstream inner_file;
fstream outer_file;
fstream params_file;
fstream lms_file;
fstream lms_remover_file;
fstream laplace_file;
ifstream p300_infile;
ifstream exercise_infile;

void saveParam(){
	params_file << "Gains: "    << "\n"
		    << outer_exercise_gain << "\n"
		    << outer_p300_gain << "\n"
		    << inner_exercise_gain << "\n"
		    << inner_p300_gain << "\n"
#ifdef DoDeepLearning
		    << remover_exercise_gain << "\n"
		    << remover_p300_gain << "\n"
		    << feedback_p300_gain << "\n"
		    << "Etas: " << "\n"
		    << w_eta_p300 << "\n"
		    << b_eta_p300 << "\n"
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
#endif
		    << "LMS" << "\n"
		    << LMS_COEFF << "\n"
		    << LMS_LEARNING_RATE << "\n";

#ifdef doOuterPreFilter
	params_file    << "didNoisePreFilter" << "\n"
		       << maxFilterLength << "\n";
#endif
#ifdef doInnerPreFilter
	params_file    << "didSignalPreFilter" << "\n"
		       << maxFilterLength << "\n";
#endif
#ifdef doOuterDelayLine
	params_file    << "didOuterDelayLine" << "\n"
		       << outerDelayLineLength << "\n";
#endif
#ifdef doInnerDelay
	params_file    << "didSignalDelay" << "\n"
		       << innerDelayLineLength << "\n";
#endif
}

void freeMemory(){
#ifdef DoShowPlots
	delete plots;
#endif
#ifdef DoDeepLearning
	delete NNO;
#endif
#ifdef doOuterPreFilter
	delete outer_filter;
#endif
#ifdef doInnerPreFilter
	delete inner_filter;
#endif
	delete lms_filter_p300;
}

void handleFiles(){
	params_file.close();
#ifdef DoDeepLearnig
	weight_p300_file.close();
	remover_file.close();
	nn_file.close();
#endif
	inner_file.close();
	outer_file.close();
	lms_file.close();
	laplace_file.close();
	lms_remover_file.close();
}

int main(int argc, const char *argv[]) {
	std::srand(1);
	for (int k = 0; k < num_subjects; k++) {
		int SUBJECT = k+1;
		cout << "subject: " << SUBJECT << endl;
		int count = 0;
		//setting up the interactive window and the dynamic plot class
#ifdef DoShowPlots
		auto frame = cv::Mat(cv::Size(plotW, plotH), CV_8UC3);
		cvui::init(WINDOW, 1);
		plots = new dynaPlots(frame, plotW, plotH);
#endif
		//create files for saving the data and parameters
		string sbjct = std::to_string(SUBJECT);
#ifdef DoDeepLearning
		nn_file.open("./cppData/subject" + sbjct + "/fnn_subject" + sbjct + ".tsv", fstream::out);
		remover_file.open("./cppData/subject" + sbjct + "/remover_subject" + sbjct + ".tsv", fstream::out);
		weight_p300_file.open("./cppData/subject" + sbjct + "/lWeights_open_subject" + sbjct + ".tsv", fstream::out);
#endif
		inner_file.open("./cppData/subject" + sbjct + "/inner_subject" + sbjct + ".tsv", fstream::out);
		outer_file.open("./cppData/subject" + sbjct + "/outer_subject" + sbjct + ".tsv", fstream::out);
		params_file.open("./cppData/subject" + sbjct + "/cppParams_subject" + sbjct + ".tsv", fstream::out);
		lms_file.open("./cppData/subject" + sbjct + "/lmsOutput_subject" + sbjct + ".tsv", fstream::out);
		lms_remover_file.open("./cppData/subject" + sbjct + "/lmsCorrelation_subject" + sbjct + ".tsv", fstream::out);
		laplace_file.open("./cppData/subject" + sbjct + "/laplace_subject" + sbjct + ".tsv", fstream::out);

		if (!params_file) {
			cout << "Unable to create files";
			exit(1); // terminate with error
		}

		char tmp[256];
		sprintf(tmp,"../noisewalls/EEG_recordings/participant%03d/rawp300.tsv",SUBJECT);
		p300_infile.open(tmp);
		if (!p300_infile) {
			cout << "Unable to open file: " << tmp << endl;
			exit(1); // terminate with error
		}

		//setting up all the filters required
#ifdef doOuterPreFilter
		outer_filter= new Fir1("./pyFiles/forOuter.dat");
		outer_filter->reset();
#endif
#ifdef doInnerPreFilter
		inner_filter= new Fir1("./pyFiles/forInner.dat");
		inner_filter->reset();
#endif
#ifdef doOuterPreFilter
		int waitOutFilterDelay = maxFilterLength;
#else
#ifdef doInnerPreFilter
		int waitOutFilterDelay = maxFilterLength;
#else
		int waitOutFilterDelay = 1;
#endif
#endif

		lms_filter_p300 = new Fir1(LMS_COEFF);
		lms_filter_p300->setLearningRate(LMS_LEARNING_RATE);

		double corrLMS_p300 = 0;
		double lms_output_p300 = 0;
//    const float fs = 250;

		//setting up the neural networks
#ifdef DoDeepLearning
		NNO->initNetwork(Neuron::W_RANDOM, Neuron::B_RANDOM, Neuron::Act_Sigmoid);
#endif

		while (!p300_infile.eof()) {
			count += 1;
			//get the data from .tsv files:
			p300_infile >> inner_p300_raw_data >> outer_p300_raw_data >> p300trigger;
			// GET ALL GAINS:
#ifdef DoDeepLearning
#ifdef DoShowPlots
			inner_p300_gain = plots->get_inner_gain(1);
			outer_p300_gain = plots->get_outer_gain(1);
			remover_p300_gain = plots->get_remover_gain(1);
			feedback_p300_gain = plots->get_feedback_gain(1);
#else
			inner_p300_gain = 100;
			outer_p300_gain = 100;
			remover_p300_gain = 10;
			feedback_p300_gain = 1;
#endif
#endif
			//A) INNER ELECTRODE:
			//1) ADJUST & AMPLIFY
			double inner_p300_raw = inner_p300_gain * inner_p300_raw_data;
#ifdef doInnerPreFilter
			double inner_p300_filtered = inner_filter->filter(inner_p300_raw);
#else
			double inner_p300_filtered = inner_p300_raw;
#endif
			//3) DELAY
#ifdef doInnerDelay
			inner_p300_delayLine.push_back(inner_p300_filtered);
			double inner_p300 = inner_p300_delayLine[0];
			
			inner_p300trigger_delayLine.push_back(p300trigger);
			double delayedp300trigger = inner_p300trigger_delayLine[0];
#else
			double inner_p300 = inner_p300_filtered;
#endif
			//B) OUTER ELECTRODE:
			//1) ADJUST & AMPLIFY
			double outer_p300_raw = outer_p300_gain * outer_p300_raw_data;
#ifdef doOuterPreFilter
			double outer_p300 = outer_filter->filter(outer_p300_raw);
#else
			double outer_p300 = outer_p300_raw;
#endif
			//3) DELAY LINE
			for (int i = outerDelayLineLength-1 ; i > 0; i--){
				outer_p300_delayLine[i] = outer_p300_delayLine[i-1];

			}
			outer_p300_delayLine[0] = outer_p300;
			double* outer_p300_delayed = &outer_p300_delayLine[0];

			// OUTER INPUT TO NETWORK
#ifdef DoDeepLearning
			NNO->setInputs(outer_p300_delayed);
			NNO->propInputs();

			// REMOVER OUTPUT FROM NETWORK
			double remover_p300 = NNO->getOutput(0) * remover_p300_gain;
			double f_nn_p300 = (inner_p300 - remover_p300) * feedback_p300_gain;

			// FEEDBACK TO THE NETWORK 
			NNO->setErrorCoeff(0, 1, 0, 0, 0, 0); //global, back, mid, forward, local, echo error
			NNO->setBackwardError(f_nn_p300);
			NNO->propErrorBackward();
#endif

			// LEARN
#ifdef DoDeepLearning
#ifdef DoShowPlots
			w_eta_p300 = plots->get_wEta(1);
			b_eta_p300 = plots->get_bEta(1);
#else
			w_eta_p300 = 1;
			b_eta_p300 = 2;
#endif
#endif

#ifdef DoDeepLearning
			NNO->setLearningRate(w_eta_p300, b_eta_p300);
			//if (count > maxFilterLength+outerDelayLineLength){
			NNO->updateWeights();
			//}
			// SAVE WEIGHTS
			for (int i = 0; i < NLAYERS; i++) {
				weight_p300_file << NNO->getLayerWeightDistance(i) << " ";
			}
			weight_p300_file << NNO->getWeightDistance() << "\n";
			NNO->snapWeights("cppData", "p300", SUBJECT);
			double l1_o = NNO->getLayerWeightDistance(0);
			double l2_o = NNO->getLayerWeightDistance(1);
			double l3_o = NNO->getLayerWeightDistance(2);
#endif

			// Do Laplace filter
			double laplace_p300 = inner_p300 - outer_p300;


			// Do LMS filter
			corrLMS_p300 += lms_filter_p300->filter(outer_p300);
			lms_output_p300 = inner_p300 - corrLMS_p300;
			lms_filter_p300->lms_update(lms_output_p300);

			// SAVE SIGNALS INTO FILES
			laplace_file << laplace_p300 << "\t" << delayedp300trigger << endl;
			inner_file << inner_p300 << endl;
#ifdef DoDeepLearning
			remover_file << remover_p300 << endl;
			nn_file << f_nn_p300 << "\t" << delayedp300trigger << endl;
#endif
			lms_file << lms_output_p300 << "\t" << delayedp300trigger << endl;
			lms_remover_file << corrLMS_p300 << endl;

			// PUT VARIABLES IN BUFFERS
			// 1) MAIN SIGNALS
			oo_buf.push_back(outer_p300_delayLine[0]);
			oo_end_buf.push_back(outer_p300_delayLine[outerDelayLineLength-1]);
			oo_raw_buf.push_back(outer_p300_raw);
			io_buf.push_back(inner_p300);
			io_raw_buf.push_back(inner_p300_raw);
#ifdef DoDeepLearning
			ro_buf.push_back(remover_p300);
			f_nno_buf.push_back(f_nn_p300);
#endif
			// 2) LAYER WEIGHTS
#ifdef DoDeepLearning
			l1_o_buf.push_back(l1_o);
			l2_o_buf.push_back(l2_o);
			l3_o_buf.push_back(l3_o);
#endif

			// 3) LMS outputs
			lms_o_buf.push_back(lms_output_p300);

			// PUTTING BUFFERS IN VECTORS FOR PLOTS
			// 1) MAIN SIGNALS
			//      EXERCISE EYES
			std::vector<double> oo_plot(oo_buf.begin(), oo_buf.end());
			std::vector<double> oo_end_plot(oo_end_buf.begin(), oo_end_buf.end());
			std::vector<double> oo_raw_plot(oo_raw_buf.begin(), oo_raw_buf.end());
			std::vector<double> io_plot(io_buf.begin(), io_buf.end());
			std::vector<double> io_raw_plot(io_raw_buf.begin(), io_raw_buf.end());
#ifdef DoDeepLearning
			std::vector<double> ro_plot(ro_buf.begin(), ro_buf.end());
			std::vector<double> f_nno_plot(f_nno_buf.begin(), f_nno_buf.end());
			// 2) LAYER WEIGHTS
			std::vector<double> l1_o_plot(l1_o_buf.begin(), l1_o_buf.end());
			std::vector<double> l2_o_plot(l2_o_buf.begin(), l2_o_buf.end());
			std::vector<double> l3_o_plot(l3_o_buf.begin(), l3_o_buf.end());
#endif
			// LMS outputs
			std::vector<double> lms_o_plot(lms_o_buf.begin(), lms_o_buf.end());

			int endTime = time(nullptr);
			int duration = endTime - startTime;

#ifdef DoShowPlots
			frame = cv::Scalar(60, 60, 60);
#ifndef DoDeepLearning
			std::vector<double> ro_plot = {0};
			std::vector<double> f_nno_plot = {0};
			std::vector<double> l1_o_plot = {0};
			std::vector<double> l2_o_plot = {0};
			std::vector<double> l3_o_plot = {0};
#endif
			plots->plotMainSignals(oo_raw_plot, oo_plot, oo_end_plot,
					       io_raw_plot, io_plot, ro_plot, f_nno_plot,
					       l1_o_plot, l2_o_plot, l3_o_plot, lms_o_plot, 1);
			plots->plotVariables();
			plots->plotTitle(count, duration);
			cvui::update();
			cv::imshow(WINDOW, frame);
#endif

/**
 * If the Esc button is pressed on the interactive window the final SNRs are printed on the console and
 * all SNRs and parameters are saved to a file. Also all pointers are deleted to free dynamically allocated memory.
 * Then the files are closed and program returns with 0.
 */
			if (cv::waitKey(20) == ESC_key) {
				saveParam();
#ifdef DoDeepLearning
				NNO->snapWeights("cppData", "p300", SUBJECT);
#endif
				handleFiles();
				freeMemory();
				return 0;
			}
		}
		saveParam();
		handleFiles();
		exercise_infile.close();
		p300_infile.close();
		cout << "The program has reached the end of the input file" << endl;
	}
	freeMemory();
}

