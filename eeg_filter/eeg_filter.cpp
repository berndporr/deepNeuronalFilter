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
#include "parameters.h"
#include "dynamicPlots.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

using namespace std;
constexpr int ESC_key = 27;

// PLOTTING
#define WINDOW "Deep Neuronal Filter"
const int plotW = 1200;
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



void processOneSubject(const int subjIndex, const char* tasksubdir = nullptr, const bool showPlots = true) {
	auto starttime = chrono::high_resolution_clock::now();
	
	std::srand(1);

	// file path prefix for the results
	std::string outpPrefix = "p300";

	int fs = 250;
	if (nullptr != tasksubdir) {
		fs = 500;
		outpPrefix = tasksubdir;
	}

	fprintf(stderr,"Starting DNF on subj %d, filename = %s.\n",subjIndex, outpPrefix.c_str());

	const int samplesNoLearning = 3 * fs / innerHighpassCutOff;
	
	const int nTapsDNF = fs / outerHighpassCutOff;
	fprintf(stderr,"nTapsDNF = %d\n",nTapsDNF);
	
	boost::circular_buffer<double> oo_buf(bufferLength);
	boost::circular_buffer<double> io_buf(bufferLength);
	boost::circular_buffer<double> ro_buf(bufferLength);
	boost::circular_buffer<double> f_nno_buf(bufferLength);
//LMS
	boost::circular_buffer<double> lms_o_buf(bufferLength);
	boost::circular_buffer<double> lms_r_buf(bufferLength);
	
// FILES
	fstream dnf_file;
	fstream inner_file;
	fstream outer_file;
	fstream lms_file;
	fstream laplace_file;
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

	DNF dnf(NLAYERS,nTapsDNF,fs,ACTIVATION);

	//adding delay line for the noise
	boost::circular_buffer<double> innertrigger_delayLine(dnf.getSignalDelaySteps());
		
	dnf_file.open(outpPrefix+"/subject" + sbjct + "/dnf.tsv", fstream::out);
	inner_file.open(outpPrefix+"/subject" + sbjct + "/inner.tsv", fstream::out);
	outer_file.open(outpPrefix+"/subject" + sbjct + "/outer.tsv", fstream::out);
	lms_file.open(outpPrefix+"/subject" + sbjct + "/lms.tsv", fstream::out);
	laplace_file.open(outpPrefix+"/subject" + sbjct + "/laplace.tsv", fstream::out);
#ifdef SAVE_WEIGHTS
	weight_file.open(outpPrefix+"/subject" + sbjct + "/lWeights.tsv", fstream::out);
#endif
	wdistance_file.open(outpPrefix+"/subject" + sbjct + "/weight_distance.tsv", fstream::out);
	
	char fullpath2data[256];
	if (nullptr != tasksubdir) {
		sprintf(fullpath2data,tasksPath,subjIndex,tasksubdir);
	} else {
		sprintf(fullpath2data,p300Path,subjIndex);
	}
	p300_infile.open(fullpath2data);
	if (!p300_infile) {
		cout << "Unable to open file: " << fullpath2data << endl;
		exit(1); // terminate with error
	}
	
	//setting up all the filters required
	Iir::Butterworth::HighPass<filterorder> outer_filterHP;
	outer_filterHP.setup(fs,outerHighpassCutOff);
	Iir::Butterworth::BandStop<filterorder> outer_filterBS;
	outer_filterBS.setup(fs,powerlineFrequ,bsBandwidth);
	Iir::Butterworth::HighPass<filterorder> inner_filterHP;
	inner_filterHP.setup(fs,innerHighpassCutOff);
	Iir::Butterworth::BandStop<filterorder> inner_filterBS;
	inner_filterBS.setup(fs,powerlineFrequ,bsBandwidth);

	Iir::Butterworth::HighPass<filterorder> laplaceHP;
	laplaceHP.setup(fs,LaplaceCutOff);
	Iir::Butterworth::BandStop<filterorder> laplaceBS;
	laplaceBS.setup(fs,powerlineFrequ,bsBandwidth);
	
	Fir1 lms_filter(nTapsDNF);
	
	fprintf(stderr,"inner_gain = %f, outer_gain = %f, remover_gain = %f\n",inner_gain,outer_gain,remover_gain);

	// main loop processsing sample by sample
	while (!p300_infile.eof()) {
		count++;
		//get the data from .tsv files:

		//SIGNALS
		double inner_raw_data = 0;
		double outer_raw_data = 0;
		double p300trigger = 0;
		
		if (nullptr == tasksubdir) {
			p300_infile >> inner_raw_data >> outer_raw_data >> p300trigger;
		} else {
			std::string line;
			std::getline(p300_infile, line);
			vector<string> row_values;
			split(line, '\t', row_values);
			if (row_values.size()>7) {
				inner_raw_data = boost::lexical_cast<double>(row_values[7]);
				outer_raw_data = boost::lexical_cast<double>(row_values[8]);
			}
			p300trigger = 0;
		}
		
		//A) INNER ELECTRODE:
		//1) ADJUST & AMPLIFY
		const double inner_raw = inner_gain * inner_raw_data;
		double inner_filtered = inner_filterHP.filter(inner_raw);
		inner_filtered = inner_filterBS.filter(inner_filtered);

		innertrigger_delayLine.push_back(p300trigger);
		const double delayedp300trigger = innertrigger_delayLine[0];

		//B) OUTER ELECTRODE:
		//1) ADJUST & AMPLIFY
		const double outer_raw = outer_gain * outer_raw_data;
		const double outerhp = outer_filterHP.filter(outer_raw);
		const double outer = outer_filterBS.filter(outerhp);

		double f_nn = dnf.filter(inner_filtered,outer);

		double w_eta = dnf_learning_rate_p300;
		if (nullptr != tasksubdir) {
			w_eta = dnf_learning_rate_tasks;
		}
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

		// Do Laplace filter
		double laplace = laplaceHP.filter(inner_raw_data - outer_raw_data);
		laplace = laplaceBS.filter(laplace);

		// Do LMS filter
		if (count > (samplesNoLearning+nTapsDNF)){
			if (nullptr == tasksubdir) {
				lms_filter.setLearningRate(lms_learning_rate_p300);
			} else {
				lms_filter.setLearningRate(lms_learning_rate_tasks);
			}
		} else {
			lms_filter.setLearningRate(0);
		}
		double corrLMS = lms_filter.filter(outer);
		double lms_output = dnf.getDelayedSignal() - corrLMS;
		if (count > (samplesNoLearning+nTapsDNF)){
			lms_filter.lms_update(lms_output);
		}
		
		// SAVE SIGNALS INTO FILES
		laplace_file << laplace << "\t" << p300trigger << endl;
		// undo the gain so that the signal is again in volt
		inner_file << dnf.getDelayedSignal()/inner_gain << "\t" << delayedp300trigger << endl;
		outer_file << outer/outer_gain << "\t" << delayedp300trigger << endl;
		dnf_file << dnf.getOutput()/inner_gain << "\t" << dnf.getRemover()/inner_gain << "\t" << delayedp300trigger << endl;
		lms_file << lms_output/inner_gain << "\t" << corrLMS/inner_gain << "\t" << delayedp300trigger << endl;
		
		// PUT VARIABLES IN BUFFERS
		// 1) MAIN SIGNALS
		oo_buf.push_back(outer);
		io_buf.push_back(dnf.getDelayedSignal());
		ro_buf.push_back(dnf.getRemover());
		f_nno_buf.push_back(f_nn);
		// 2) LMS outputs
		lms_o_buf.push_back(lms_output);
		lms_r_buf.push_back(corrLMS);
		
		// PUTTING BUFFERS IN VECTORS FOR PLOTS
		// MAIN SIGNALS
		std::vector<double> oo_plot(oo_buf.begin(), oo_buf.end());
		std::vector<double> io_plot(io_buf.begin(), io_buf.end());
		std::vector<double> ro_plot(ro_buf.begin(), ro_buf.end());
		std::vector<double> f_nno_plot(f_nno_buf.begin(), f_nno_buf.end());
		// LMS outputs
		std::vector<double> lms_o_plot(lms_o_buf.begin(), lms_o_buf.end());
		std::vector<double> lms_r_plot(lms_r_buf.begin(), lms_r_buf.end());
		
		if (plots) {
			frame = cv::Scalar(60, 60, 60);
			if (0 == (count % 10)) {
				plots->plotMainSignals(oo_plot,
						       io_plot,
						       ro_plot,
						       f_nno_plot,
						       lms_r_plot,
						       lms_o_plot);
				plots->plotTitle(sbjct, count, round(count / fs),fullpath2data);
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
	dnf_file.close();
	inner_file.close();
	outer_file.close();
	lms_file.close();
	laplace_file.close();
	wdistance_file.close();
#ifdef SAVE_WEIGHTS
	weight_file.close();
#endif
	if (plots) delete plots;
	auto stoptime = chrono::high_resolution_clock::now();
	auto time_taken = stoptime - starttime;
	cout << "It took " << chrono::duration_cast<chrono::seconds>(time_taken).count() << " sec to run the program." << endl;
}



int main(int argc, const char *argv[]) {
	if (argc < 2) {
		fprintf(stderr,"Usage: %s [-a] <subjectNumber> [tasksubdir]\n",argv[0]);
		fprintf(stderr,"       -a calculates all 20 subjects in a loop.\n");
		fprintf(stderr,"       -b calculates all 20 subjects multi-threaded without screen output.\n");
		fprintf(stderr,"       Press ESC in the plot window to cancel the program.\n");
		return 0;
	}
	
	const char *tasksubdir = nullptr;
	if (argc > 2) {
		tasksubdir = argv[2];
	}

	if (strcmp(argv[1],"-a") == 0) {
		for(int i = 0; i < nSubj; i++) {
			processOneSubject(i+1,tasksubdir);
		}
		return 0;
	}
	
	if (strcmp(argv[1],"-b") == 0) {
		std::thread* workers[nSubj];
		for(int i = 0; i < nSubj; i++) {
			workers[i] = new std::thread(processOneSubject,i+1,tasksubdir,false);
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
	processOneSubject(subj,tasksubdir);
	return 0;
}
