#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
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
#include <random>
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

void runSimulation(const float duration,
		   const bool showPlots = true,
		   const int experimentNumber = -1,
		   const float alpha = 0.1,
		   const float noiseAmplitude = 25) {
	std::srand(1);

	// file path prefix for the results
	std::string outpPrefix = "results";
	mkdir(outpPrefix.c_str(), S_IRWXU);

	if (experimentNumber != -1) {
		outpPrefix = "results/"+std::to_string(experimentNumber);
		mkdir(outpPrefix.c_str(), S_IRWXU);
	}

	int fs = 500;
	int n = int(fs * duration);

	const int samplesNoLearning = 3 * fs / innerHighpassCutOff;
	
	const int nTapsDNF = fs / outerHighpassCutOff;
	
	boost::circular_buffer<double> oo_buf(bufferLength);
	boost::circular_buffer<double> io_buf(bufferLength);
	boost::circular_buffer<double> ro_buf(bufferLength);
	boost::circular_buffer<double> f_nno_buf(bufferLength);
//LMS
	boost::circular_buffer<double> lms_o_buf(bufferLength);
	boost::circular_buffer<double> lms_r_buf(bufferLength);
	
// FILES
	fstream signal_file;
	fstream noise_file;
	fstream dnf_file;
	fstream inner_file;
	fstream outer_file;
	fstream inner_raw_file;
	fstream outer_raw_file;
	fstream lms_file;
	fstream laplace_file;
	fstream wdistance_file;

	//setting up the interactive window and the dynamic plot class
	auto frame = cv::Mat(cv::Size(plotW, plotH), CV_8UC3);
	dynaPlots* plots = NULL;
	if (showPlots) {
		cvui::init(WINDOW, 1);
		plots = new dynaPlots(frame, plotW, plotH);
	}

	DNF dnf(NLAYERS,nTapsDNF,fs,ACTIVATION);

	//adding delay line for the noise
	boost::circular_buffer<double> innertrigger_delayLine(dnf.getSignalDelaySteps());

	signal_file.open(outpPrefix+"/signal.tsv", fstream::out);
	noise_file.open(outpPrefix+"/noise.tsv", fstream::out);
	dnf_file.open(outpPrefix+"/dnf.tsv", fstream::out);
	inner_file.open(outpPrefix+"/inner.tsv", fstream::out);
	outer_file.open(outpPrefix+"/outer.tsv", fstream::out);
	inner_raw_file.open(outpPrefix+"/inner_raw.tsv", fstream::out);
	outer_raw_file.open(outpPrefix+"/outer_raw.tsv", fstream::out);
	lms_file.open(outpPrefix+"/lms.tsv", fstream::out);
	laplace_file.open(outpPrefix+"/laplace.tsv", fstream::out);
	wdistance_file.open(outpPrefix+"/weight_distance.tsv", fstream::out);
	
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
	
	std::random_device rd_r;
	std::mt19937 gen_r(rd_r());
 	std::normal_distribution<> d_r(0,noiseAmplitude); // uV
	Iir::Butterworth::BandPass<filterorder> rBP;
	rBP.setup(fs,noiseModelBandpassCenter,noiseModelBandpassWidth);

	std::random_device rd_c;
	std::mt19937 gen_c(rd_c());
 	std::normal_distribution<> d_c(0,signalAmplitude); // uV
	Iir::Butterworth::LowPass<filterorder> cLP;
	cLP.setup(fs,signalModelLowpassFreq);

	fprintf(stderr,
		"Starting DNF: subdir = %s, inner_gain = %f, outer_gain = %f, "
		"remover_gain = %f, alpha = %f, noiseAmplitude = %f uV, "
		"signalAmplitude = %f uV, nTapsDNF = %d\n",
		outpPrefix.c_str(),
		inner_gain,
		outer_gain,
		remover_gain,
		alpha,
		noiseAmplitude,
		signalAmplitude,
		nTapsDNF);

	const long jawclenchSampleDistance = jawclenchEverySec * fs;
	const long jawclenchSampleDuration = jawclenchDuration * fs;

	long jawclenchDistanceCounter = jawclenchSampleDistance;
	long jawclenchDurationCounter = 0;

	// main loop processsing sample by sample
	for (long count=0; count<n; count++) {
		double p300trigger = 0;

		double r = d_r(gen_r) / 1E6;
		r = rBP.filter(r);
		noise_file << r << endl;

		if (jawclenchDistanceCounter > 0) {
			jawclenchDistanceCounter--;
		} else {
			jawclenchDistanceCounter = jawclenchSampleDistance;
			jawclenchDurationCounter = jawclenchSampleDuration;
		}

		if (jawclenchDurationCounter > 0) {
			jawclenchDurationCounter--;
			r = r * jawclenchNoiseBoost;
		}

		double c = d_c(gen_c) / 1E6;
		c = cLP.filter(c);
		signal_file << c << endl;

		double inner_raw_data = r + c;
		double outer_raw_data = r + alpha * c;

		inner_raw_file << inner_raw_data << endl;
		outer_raw_file << outer_raw_data << endl;
		
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

		float w_eta = dnf_learning_rate;
		if (count > (samplesNoLearning+nTapsDNF)){
			dnf.getNet().setLearningRate(w_eta, 0);
		} else {
			dnf.getNet().setLearningRate(0, 0);
		}
		
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
			lms_filter.setLearningRate(lms_learning_rate);
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
				plots->plotTitle("EEG sim", count, round(count / fs),"");
				cvui::update();
				cv::imshow(WINDOW, frame);
				
				if (cv::waitKey(1) == ESC_key) {
					break;
				}
			}
		}
	}
	signal_file.close();
	noise_file.close();
	dnf_file.close();
	inner_file.close();
	outer_file.close();
	inner_raw_file.close();
	outer_raw_file.close();
	lms_file.close();
	laplace_file.close();
	wdistance_file.close();
	if (plots) delete plots;
	cout << "Sim finished." << endl;
}




int main(int argc, const char *argv[]) {
	bool screenoutput = true;
	
	if (argc < 2) {
		fprintf(stderr,"Usage: %s [-a] [-b]\n",argv[0]);
		fprintf(stderr,"       -a disables screen output.\n");
		fprintf(stderr,"       -b calculates 20 simulations multi-threaded without screen output.\n");
		fprintf(stderr,"       Press ESC in the plot window to cancel the program.\n");
	} else {
		if (strcmp(argv[1],"-a") == 0) {
			screenoutput = false;
		}
		if (strcmp(argv[1],"-b") == 0) {
			std::thread* workers[nExperiments];
			std::random_device rd_noiseAmplitude;
			std::mt19937 gen_noiseAmplitude(rd_noiseAmplitude());
			std::normal_distribution<> d_noiseAmplitude(averageNoiseAmplitude,
								    standardDevNoiseAmplitude); // uV
			for(int i = 0; i < nExperiments; i++) {
				float noiseAmplitude = d_noiseAmplitude(gen_noiseAmplitude);
				workers[i] = new std::thread(runSimulation,duration,false,i,alpha,noiseAmplitude);
			}
			for(int i = 0; i < nExperiments; i++) {
				workers[i]->join();
				delete workers[i];
			}
			return 0;
		}
	}

	std::cout << "Running a single simulation" << endl;
	runSimulation(duration,screenoutput,-1,alpha,averageNoiseAmplitude);
	return 0;
}
