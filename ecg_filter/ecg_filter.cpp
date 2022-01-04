#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <boost/circular_buffer.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/lexical_cast.hpp>
#include <Iir.h>
#include <Fir1.h>
#include <chrono>
#include <string>
#include <ctime>
#include <thread>         // std::thread
#include <memory>
#include <numeric>
#include "dnf.h"
#include "parameters.h"
#include "dynamicPlots.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

using namespace std;
constexpr int ESC_key = 27;

// PLOTTING
#define WINDOW "Deep Neuronal Filter"
const int plotW = 1200/2;
const int plotH = 720;


// take from http://stackoverflow.com/a/236803/248823
void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}



void processOneSubject(const int subjIndex, const char* filename, const bool showPlots = true) {
	std::srand(1);

	// file path prefix for the results
	std::string outpPrefix = "results";

	const int fs = 250;
	if (NULL != filename) {
		outpPrefix = filename;
	}

	const int samplesNoLearning = 3 * fs / ecgemgHighpassCutOff;
	
	const int nTapsDNF = fs / emgnoiseHighpassCutOff;
	fprintf(stderr,"nTapsDNF = %d\n",nTapsDNF);
	
	boost::circular_buffer<double> oo_buf(bufferLength);
	boost::circular_buffer<double> io_buf(bufferLength);
	boost::circular_buffer<double> ro_buf(bufferLength);
	boost::circular_buffer<double> f_nno_buf(bufferLength);
//LMS
	boost::circular_buffer<double> lms_o_buf(bufferLength);
	
// FILES
	fstream nn_file;
	fstream remover_file;
	fstream ecgemg_file;
	fstream emgnoise_file;
	fstream lms_file;
	fstream lms_remover_file;
	ifstream p300_infile;
	fstream wdistance_file;
#ifdef SAVE_WEIGHTS
	fstream weight_file;
#endif

	long count = 0;
	
	//setting up the interactive window and the dynamic plot class
	auto frame = cv::Mat(cv::Size(plotW, plotH), CV_8UC3);
	dynaPlots* plots = NULL;
	if (showPlots) {
		cvui::init(WINDOW, 1);
		plots = new dynaPlots(frame, plotW, plotH);
	}

	//create files for saving the data and parameters
	string sbjct = std::to_string(subjIndex);

	DNF dnf(NLAYERS,nTapsDNF,fs);

	//adding delay line for the noise
	boost::circular_buffer<double> ecgemgtrigger_delayLine(dnf.getSignalDelaySteps());
		
	nn_file.open(outpPrefix+"/subject" + sbjct + "/fnn.tsv", fstream::out);
	remover_file.open(outpPrefix+"/subject" + sbjct + "/remover.tsv", fstream::out);
	ecgemg_file.open(outpPrefix+"/subject" + sbjct + "/ecgemg.tsv", fstream::out);
	emgnoise_file.open(outpPrefix+"/subject" + sbjct + "/emgnoise.tsv", fstream::out);
	lms_file.open(outpPrefix+"/subject" + sbjct + "/lmsOutput.tsv", fstream::out);
	lms_remover_file.open(outpPrefix+"/subject" + sbjct + "/lmsCorrelation.tsv", fstream::out);
#ifdef SAVE_WEIGHTS
	weight_file.open(outpPrefix+"/subject" + sbjct + "/lWeights.tsv", fstream::out);
#endif
	wdistance_file.open(outpPrefix+"/subject" + sbjct + "/weight_distance.tsv", fstream::out);
	
	char tmp[256];
	if (NULL != filename) {
		sprintf(tmp,"../../ecgemg/participant%d/%s.tsv",subjIndex,filename);
	} else {
		sprintf(tmp,"../../ecgemg/participant%d/stress1.tsv",subjIndex);
	}
	p300_infile.open(tmp);
	if (!p300_infile) {
		cout << "Unable to open file: " << tmp << endl;
		exit(1); // terminate with error
	}
	
	//setting up all the filters required
	Iir::Butterworth::HighPass<filterorder> emgnoise_filterHP;
	emgnoise_filterHP.setup(fs,emgnoiseHighpassCutOff);
	Iir::Butterworth::BandStop<filterorder> emgnoise_filterBS;
	emgnoise_filterBS.setup(fs,powerlineFrequ,bsBandwidth);
	Iir::Butterworth::HighPass<filterorder> ecgemg_filterHP;
	ecgemg_filterHP.setup(fs,ecgemgHighpassCutOff);
	Iir::Butterworth::BandStop<filterorder> ecgemg_filterBS;
	ecgemg_filterBS.setup(fs,powerlineFrequ,bsBandwidth);

	Fir1 lms_filter(nTapsDNF);
	lms_filter.setLearningRate(LMS_LEARNING_RATE);
	
	fprintf(stderr,"ecgemg_gain = %f, emgnoise_gain = %f, remover_gain = %f\n",ecgemg_gain,emgnoise_gain,remover_gain);

	// main loop processsing sample by sample
	while (!p300_infile.eof()) {
		count++;
		//get the data from .tsv files:

		//SIGNALS
		double ecgemg_raw_data = 0, emgnoise_raw_data = 0, p300trigger = 0;
		std::string line;
		std::getline(p300_infile, line);
		vector<string> row_values;
		split(line, '\t', row_values);
		if (row_values.size()>3) {
			ecgemg_raw_data = boost::lexical_cast<double>(row_values[1]);
			emgnoise_raw_data = boost::lexical_cast<double>(row_values[2]);
		}
		p300trigger = 0;
		
		//A) ECGEMG ELECTRODE:
		//1) ADJUST & AMPLIFY
		const double ecgemg_raw = ecgemg_gain * ecgemg_raw_data;
		double ecgemg_filtered = ecgemg_filterHP.filter(ecgemg_raw);
		ecgemg_filtered = ecgemg_filterBS.filter(ecgemg_filtered);

		ecgemgtrigger_delayLine.push_back(p300trigger);
		const double delayedp300trigger = ecgemgtrigger_delayLine[0];

		//B) EMGNOISE ELECTRODE:
		//1) ADJUST & AMPLIFY
		const double emgnoise_raw = emgnoise_gain * emgnoise_raw_data;
		const double emgnoisehp = emgnoise_filterHP.filter(emgnoise_raw);
		const double emgnoise = emgnoise_filterBS.filter(emgnoisehp);

		double f_nn = dnf.filter(ecgemg_filtered,emgnoise);
		
		if (count > (samplesNoLearning+nTapsDNF)){
			dnf.getNet().setLearningRate(w_eta, 0);
		} else {
			dnf.getNet().setLearningRate(0, 0);
		}
		
#ifdef SAVE_WEIGHTS
		// SAVE WEIGHTS
		NNO.snapWeights(outpPrefix, "p300", subjIndex);
#endif
		wdistance_file << dnf.getNet().getWeightDistance();
		for(int i=0; i < NLAYERS; i++ ) {
			wdistance_file << "\t" << dnf.getNet().getLayerWeightDistance(i);
		}
		wdistance_file << endl;

		// Do LMS filter
		double corrLMS = lms_filter.filter(emgnoise);
		double lms_output = dnf.getDelayedSignal() - corrLMS;
		if (count > (samplesNoLearning+nTapsDNF)){
			lms_filter.lms_update(lms_output);
		}
		
		// SAVE SIGNALS INTO FILES
		// undo the gain so that the signal is again in volt
		ecgemg_file << dnf.getDelayedSignal()/ecgemg_gain << "\t" << delayedp300trigger << endl;
		emgnoise_file << emgnoise/emgnoise_gain << "\t" << delayedp300trigger << endl;
		remover_file << dnf.getRemover()/ecgemg_gain << endl;
		nn_file << dnf.getOutput()/ecgemg_gain << "\t" << delayedp300trigger << endl;
		lms_file << lms_output/ecgemg_gain << "\t" << delayedp300trigger << endl;
		lms_remover_file << corrLMS/ecgemg_gain << endl;
		
		// PUT VARIABLES IN BUFFERS
		// 1) MAIN SIGNALS
		oo_buf.push_back(emgnoise);
		io_buf.push_back(dnf.getDelayedSignal());
		ro_buf.push_back(dnf.getRemover());
		f_nno_buf.push_back(f_nn);
		// 2) LMS outputs
		lms_o_buf.push_back(lms_output);
		
		// PUTTING BUFFERS IN VECTORS FOR PLOTS
		// MAIN SIGNALS
		std::vector<double> oo_plot(oo_buf.begin(), oo_buf.end());
		std::vector<double> io_plot(io_buf.begin(), io_buf.end());
		std::vector<double> ro_plot(ro_buf.begin(), ro_buf.end());
		std::vector<double> f_nno_plot(f_nno_buf.begin(), f_nno_buf.end());
		// LMS outputs
		std::vector<double> lms_o_plot(lms_o_buf.begin(), lms_o_buf.end());
		
		if (plots) {
			frame = cv::Scalar(60, 60, 60);
			if (0 == (count % 10)) {
				plots->plotMainSignals(oo_plot,
						       io_plot,
						       ro_plot,
						       f_nno_plot,
						       lms_o_plot, 1);
				plots->plotTitle(sbjct, count, round(count / fs));
				cvui::update();
				cv::imshow(WINDOW, frame);
				
				if (cv::waitKey(1) == ESC_key) {
					break;
				}
			}
		}
	}
	dnf.getNet().snapWeights(outpPrefix, "p300", subjIndex);
	p300_infile.close();
	remover_file.close();
	nn_file.close();
	ecgemg_file.close();
	emgnoise_file.close();
	lms_file.close();
	lms_remover_file.close();
	wdistance_file.close();
#ifdef SAVE_WEIGHTS
	weight_file.close();
#endif
	if (plots) delete plots;
	cout << "The program has reached the end of the input file" << endl;
}



int main(int argc, const char *argv[]) {
	if (argc < 2) {
		fprintf(stderr,"Usage: %s [-a] <subjectNumber>\n",argv[0]);
		fprintf(stderr,"       -a calculates all 20 subjects in a loop.\n");
		fprintf(stderr,"       -b calculates all 20 subjects multi-threaded without screen output.\n");
		fprintf(stderr,"       Press ESC in the plot window to cancel the program.\n");
		return 0;
	}
	
	const char *filename = NULL;
	if (argc > 2) {
		filename = argv[2];
	}

	if (strcmp(argv[1],"-a") == 0) {
		for(int i = 0; i < nSubj; i++) {
			processOneSubject(i+1,filename);
		}
		return 0;
	}
	
	if (strcmp(argv[1],"-b") == 0) {
		std::thread* workers[nSubj];
		for(int i = 0; i < nSubj; i++) {
			workers[i] = new std::thread(processOneSubject,i+1,filename,false);
		}
		for(int i = 0; i < nSubj; i++) {
			workers[i]->join();
			delete workers[i];
		}
		return 0;
	}
	
	const int subj = atoi(argv[1]);
	if ( (subj < 1) || (subj > nSubj) ) {
		fprintf(stderr,"Subj number of out range.\n");
		return -1;
	}
	processOneSubject(subj,filename);
	return 0;
}
