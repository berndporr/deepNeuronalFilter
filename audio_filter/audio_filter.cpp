#include <iostream>
#include <fstream>
#include <filesystem>
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
#include "wavRead/wavread.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

using namespace std;
constexpr int ESC_key = 27;

void addSOXheader(fstream &f) {
	f << "; Sample Rate " << fs << endl;
	f << "; Channels 1" << endl;
}

void processOneExperiment(const int expIndex, const bool showPlots = true) {
	std::srand(1);

	// file path prefix for the results
	std::string outpPrefix = "results";

	std::filesystem::create_directory(outpPrefix);

	fprintf(stderr,"Starting DNF on experiment %d, filename = %s.\n",expIndex, outpPrefix.c_str());

	const int samplesNoLearning = 3 * fs / signalWithNoiseHighpassCutOff;
	
	fprintf(stderr,"nTapsDNF = %d\n",nTapsDNF);
	
	boost::circular_buffer<double> oo_buf(bufferLength);
	boost::circular_buffer<double> io_buf(bufferLength);
	boost::circular_buffer<double> ro_buf(bufferLength);
	boost::circular_buffer<double> f_nno_buf(bufferLength);
	//LMS
	boost::circular_buffer<double> lms_o_buf(bufferLength);
	boost::circular_buffer<double> lms_r_buf(bufferLength);

	WAVread wavread;

	long count = 0;
	
	//setting up the interactive window and the dynamic plot class
	auto frame = cv::Mat(cv::Size(plotW, plotH), CV_8UC3);
	dynaPlots* plots = NULL;
	if (showPlots) {
		cvui::init(WINDOW, 1);
		plots = new dynaPlots(frame, plotW, plotH);
	}

	//create files for saving the data and parameters
	const std::string expDir = "/exp";
	const std::string sd = outpPrefix + expDir + std::to_string(expIndex);
	std::filesystem::create_directory(sd);
	
	DNF dnf(NLAYERS,nTapsDNF,fs,ACTIVATION);

	// FILES
	fstream signalWithNoise_file;
	signalWithNoise_file.open(sd + "/signalWithNoise.dat", fstream::out);
	addSOXheader(signalWithNoise_file);

	fstream noiseref_file;
	noiseref_file.open(sd + "/noiseref.dat", fstream::out);
	addSOXheader(noiseref_file);

	fstream dnfOut_file;
	dnfOut_file.open(sd + "/dnf_out.dat", fstream::out);
	addSOXheader(dnfOut_file);

	fstream dnfRemover_file;
	dnfRemover_file.open(sd + "/dnf_remover.dat", fstream::out);
// ***add***
	addSOXheader(dnfRemover_file);

	fstream lmsOut_file;
	lmsOut_file.open(sd + "/lms_out.dat", fstream::out);
	addSOXheader(lmsOut_file);

	fstream lmsRemover_file;
	lmsRemover_file.open(sd + "/lms_remover.dat", fstream::out);
// ***add***
	addSOXheader(lmsRemover_file);

	fstream wdistance_file;
	wdistance_file.open(sd + "/weight_distance.tsv", fstream::out);
	
	char fullpath2data[256];
	sprintf(fullpath2data,audioPath,expIndex);
	int r = wavread.open(fullpath2data);
	if (r < 0) {
		cout << "Unable to open file: " << fullpath2data << endl;
		exit(1); // terminate with error
	}
	wavread.printHeaderInfo();
	
	//setting up all the filters required
	Iir::Butterworth::HighPass<filterorder> noiseref_filterHP;
	noiseref_filterHP.setup(fs,noiserefHighpassCutOff);
	Iir::Butterworth::HighPass<filterorder> signalWithNoise_filterHP;
	signalWithNoise_filterHP.setup(fs,signalWithNoiseHighpassCutOff);

	Fir1 lms_filter(nTapsDNF);
	
	fprintf(stderr,"signalWithNoise_gain = %f, noiseref_gain = %f, remover_gain = %f\n",signalWithNoise_gain,noiseref_gain,remover_gain);

	int nIgn = start_time_ignore * fs;
	for(int i = 0; (i < nIgn) && wavread.hasSample(); i++) {
		wavread.getStereoSample();
	}

	// main loop processsing sample by sample
	while (wavread.hasSample()) {
		WAVread::StereoSample s = wavread.getStereoSample();
		double signalWithNoise_raw_data = s.left; // signal + noise
		double noiseref_raw_data = s.right; // noise ref
		
		//A) SIGNALWITHNOISE ELECTRODE:
		//1) ADJUST & AMPLIFY
		const double signalWithNoise_raw = signalWithNoise_gain * signalWithNoise_raw_data;
		double signalWithNoise_filtered = signalWithNoise_filterHP.filter(signalWithNoise_raw);

		//B) NOISEREF ELECTRODE:
		//1) ADJUST & AMPLIFY
		const double noiseref_raw = noiseref_gain * noiseref_raw_data;
		const double noiserefhp = noiseref_filterHP.filter(noiseref_raw);

		double f_nn = dnf.filter(signalWithNoise_filtered,noiserefhp);

		if (count > (samplesNoLearning+nTapsDNF)){
			dnf.getNet().setLearningRate(dnf_learning_rate, 0);
		} else {
			dnf.getNet().setLearningRate(0, 0);
		}

		double t = (double)count / fs;
		
		// Write the weight distance to the file
		wdistance_file << dnf.getNet().getWeightDistance();
		for(int i=0; i < NLAYERS; i++ ) {
			wdistance_file << "\t" << dnf.getNet().getLayerWeightDistance(i);
		}
		wdistance_file << endl;

		// Do LMS filter
		if (count > (samplesNoLearning+nTapsDNF)){
			lms_filter.setLearningRate(lms_learning_rate);
		} else {
			lms_filter.setLearningRate(0);
		}
		double corrLMS = lms_filter.filter(noiserefhp);
		double lms_output = dnf.getDelayedSignal() - corrLMS;
		lms_filter.lms_update(lms_output);
		
		// SAVE SIGNALS INTO FILES
		// undo the gain so that the signal is again in volt
		signalWithNoise_file << t << " " << dnf.getDelayedSignal()/signalWithNoise_gain << endl;
		noiseref_file << t << " " << noiserefhp/noiseref_gain << " " << endl;

// ***modified***	dnfOut_file << t << " " << dnf.getOutput()/signalWithNoise_gain << endl;
		dnfOut_file << t << " " << f_nn/signalWithNoise_gain << endl;
		
		lmsOut_file << t << " " << lms_output/signalWithNoise_gain << endl;
// ***modified***	dnfRemover_file << dnf.getRemover()/signalWithNoise_gain << endl;
		dnfRemover_file << t << " " << dnf.getRemover()/signalWithNoise_gain << endl;
// ***modified***	lmsRemover_file << corrLMS/signalWithNoise_gain << endl;
		lmsRemover_file << t << " " << corrLMS/signalWithNoise_gain << endl;
		
		// PUT VARIABLES IN BUFFERS
		// 1) MAIN SIGNALS
		oo_buf.push_back(noiserefhp);
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
				plots->plotTitle(sd, count, count / fs,fullpath2data);
				cvui::update();
				cv::imshow(WINDOW, frame);
				
				if (cv::waitKey(1) == ESC_key) {
					break;
				}
			}
		}
		count++;
	}
	wavread.close();
	dnfOut_file.close();
	dnfRemover_file.close();
	signalWithNoise_file.close();
	noiseref_file.close();
	lmsOut_file.close();
	lmsRemover_file.close();
	wdistance_file.close();
	if (plots) delete plots;
	cout << "The program has reached the end of the input file" << endl;
}



int main(int argc, const char *argv[]) {
	if (argc < 2) {
		fprintf(stderr,"Usage: %s [-a] [-b] [-s] [-h] [<expNumber>]\n",argv[0]);
		fprintf(stderr,"       -a calculates all experiments one by one without screen output.\n");
		fprintf(stderr,"       -b calculates all experiments multi-threaded without screen output.\n");
		fprintf(stderr,"       -s [<expNumber>] calculates one experiment without screen output.\n");
		fprintf(stderr,"       -h show help text.\n");
		fprintf(stderr,"       Press ESC in the plot window to cancel the program.\n");
		return 0;
	}

	if (strcmp(argv[1],"-h") == 0) {
		fprintf(stderr,"Usage: %s [-a] [-b] [-s] [<expNumber>]\n",argv[0]);
		fprintf(stderr,"       -a calculates all experiments one by one without screen output.\n");
		fprintf(stderr,"       -b calculates all experiments multi-threaded without screen output.\n");
		fprintf(stderr,"       -s [<expNumber>] calculates one experiment without screen output.\n");
		fprintf(stderr,"       -h show help text.\n");
		fprintf(stderr,"       Press ESC in the plot window to cancel the program.\n");
		return 0;
	}
	
	if (strcmp(argv[1],"-a") == 0) {
		for(int i = 0; i < nExp; i++) {
			processOneExperiment(i+1,false);
		}
		return 0;
	}
	
	if (strcmp(argv[1],"-b") == 0) {
		std::thread* workers[nExp];
		for(int i = 0; i < nExp; i++) {
			workers[i] = new std::thread(processOneExperiment,i+1,false);
		}
		for(int i = 0; i < nExp; i++) {
			workers[i]->join();
			delete workers[i];
		}
		return 0;
	}

// ***add***
	if (strcmp(argv[1],"-s") == 0) {
		const int experiment = atoi(argv[2]);
		if ( (experiment < 1) || (experiment > nExp) ) {
			fprintf(stderr,"Exp number of out range.\n");
			return -1;
		}
		processOneExperiment(experiment,false);
		return 0;
	}
	
	const int experiment = atoi(argv[1]);
	if ( (experiment < 1) || (experiment > nExp) ) {
		fprintf(stderr,"Exp number of out range.\n");
		return -1;
	}
	processOneExperiment(experiment);
	return 0;
}
