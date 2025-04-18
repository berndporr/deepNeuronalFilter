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
#include <matio.h>
#include "dnf.h"
#include "parameters.h"
#include "dynamicPlots.h"
#include "matloader.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

using namespace std;
constexpr int ESC_key = 27;

// PLOTTING
#define WINDOW "Deep Neuronal Filter"
const int plotW = 1200;
const int plotH = 720;


void processOneSweep(const char* filename, const long int sweepNo, const bool showPlots = false) {

    MatLoader matloader;
    matloader.load(filename, datavarname);

    std::srand(1);

    fprintf(stderr,"Starting DNF on sweep# %ld, filename = %s.\n",sweepNo,filename);

    const int samplesNoLearning = 3 * fs / highpassCutOff;
	
    fprintf(stderr,"nTapsDNF = %d\n",nTapsDNF);

    // DNF
    boost::circular_buffer<double> oo_buf(bufferLength);
    boost::circular_buffer<double> io_buf(bufferLength);
    boost::circular_buffer<double> ro_buf(bufferLength);
    boost::circular_buffer<double> f_nno_buf(bufferLength);
    // LMS
    boost::circular_buffer<double> lms_o_buf(bufferLength);
    boost::circular_buffer<double> lms_r_buf(bufferLength);
    
    long count = 0;
	
    //setting up the interactive window and the dynamic plot class
    auto frame = cv::Mat(cv::Size(plotW, plotH), CV_8UC3);
    dynaPlots* plots = NULL;
    if (showPlots) {
	cvui::init(WINDOW, 1);
	plots = new dynaPlots(frame, plotW, plotH);
    }

    //create files for saving the data and parameters
    char tmp[20];
    sprintf(tmp,"%06ld",sweepNo);
    string strSweepNo(tmp);

    DNF dnf(NLAYERS,nTapsDNF,fs,ACTIVATION);

    //adding delay line for the noise
    boost::circular_buffer<double> innertrigger_delayLine(dnf.getSignalDelaySteps());
		
// FILES
    fstream dnf_file;
    dnf_file.open(outpPrefix+"/sweep" + strSweepNo + "_dnf.tsv", fstream::out);
    
    fstream in_file;
    in_file.open(outpPrefix+"/sweep" + strSweepNo + "_in.tsv", fstream::out);
    
    fstream lms_file;
    lms_file.open(outpPrefix+"/sweep" + strSweepNo + "_lms.tsv", fstream::out);
    
    fstream wdistance_file;
    wdistance_file.open(outpPrefix+"/sweep" + strSweepNo + "_weight_distance.tsv", fstream::out);
    
#ifdef SAVE_WEIGHTS
    fstream weight_file;
    weight_file.open(outpPrefix+"/sweep" + strSweepNo + "_lWeights.tsv", fstream::out);
#endif
	
    //setting up all the filters required
    Iir::Butterworth::HighPass<filterorder> dnfHP;
    dnfHP.setup(fs,highpassCutOff);
	
    Fir1 lms_filter(nTapsDNF);

    long int startindex = (long int)(startTime * fs);
	
    double m = 0;
    for(int count = startindex; count < matloader.getNSamples(); count++) {
	const double v = matloader.getData(sweepNo,count);
	if ( v > m ) m = v;
    }
    const double gain = 1/m;

    fprintf(stderr,"gain = %f, remover_gain = %f\n",gain,remover_gain);

    auto starttime = chrono::high_resolution_clock::now();

    // let's go
    for(int count = startindex; count < matloader.getNSamples(); count++)
    {
	const double raw = matloader.getData(sweepNo,count) * gain;
	const double filtered = dnfHP.filter(raw);
	const double refnoise1 = sin(2 * M_PI * ((double)count) * transmitter1freq / fs);
	const double refnoise2 = sin(2 * M_PI * ((double)count) * transmitter2freq / fs);
	const double refnoise = refnoise1 + refnoise2;
	
	double f_nn = dnf.filter(filtered,refnoise);

	if (count > (samplesNoLearning+nTapsDNF)){
	    dnf.getNet().setLearningRate(dnf_learning_rate, 0);
	} else {
	    dnf.getNet().setLearningRate(0, 0);
	}
	
#ifdef SAVE_WEIGHTS
	// SAVE WEIGHTS
	NNO.snapWeights(outpPrefix, "weights", sweepNo);
#endif
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

	double corrLMS = lms_filter.filter(refnoise);
	double lms_output = dnf.getDelayedSignal() - corrLMS;
	if (count > (samplesNoLearning+nTapsDNF)){
	    lms_filter.lms_update(lms_output);
	}
	
	// undo the gain so that the signal is again in volt
	in_file << dnf.getDelayedSignal()/gain << endl;
	dnf_file << dnf.getOutput()/gain << "\t" << dnf.getRemover()/gain << endl;
	lms_file << lms_output/gain << "\t" << corrLMS/gain << endl;

	// plotting
	if (plots) {
	    // PUT VARIABLES IN BUFFERS
	    // 1) MAIN SIGNALS
	    oo_buf.push_back(refnoise);
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
	
	    frame = cv::Scalar(60, 60, 60);
	    if (0 == (count % 10)) {
		plots->plotMainSignals(oo_plot,
				       io_plot,
				       ro_plot,
				       f_nno_plot,
				       lms_r_plot,
				       lms_o_plot);
		plots->plotTitle(strSweepNo, count, round(count / fs),"results");
		cvui::update();
		cv::imshow(WINDOW, frame);
		
		if (cv::waitKey(1) == ESC_key) {
		    break;
		}
	    }
	}
    }
    dnf.getNet().snapWeights(outpPrefix, "weights", sweepNo);
    dnf_file.close();
    in_file.close();
    lms_file.close();
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
    if (argc < 4) {
	fprintf(stderr,"Usage: %s matfile sweepnumber wantplots\n",argv[0]);
	return -1;
    }
    long int sweep = atoi(argv[2]);
    bool doPlot = atoi(argv[3]) > 0;
    processOneSweep(argv[1],sweep,doPlot);
    return 0;
}
