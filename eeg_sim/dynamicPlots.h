#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <boost/circular_buffer.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <Iir.h>
#include <Fir1.h>
#include <string>
#include <numeric>
#include <memory>
#include <opencv2/opencv.hpp>
#include "cvui.h"

using namespace std;

namespace cv {
	class Mat;
}

class dynaPlots{
public:
	dynaPlots(cv::Mat &_learningFrame, int _plotW, int _plotH);
	~dynaPlots();
	
	void plotMainSignals(std::vector<double> &outer,
			     std::vector<double> &inner,
			     std::vector<double> &remover,
			     std::vector<double> &fnn,
			     std::vector<double> &lms_remover,
			     std::vector<double> &lms_output
			     );
	void plotVariables();
	
	void plotTitle(std::string title, long count, int duration, const char* fullPath);
	
private:
	cv::Mat frame;
	
	int topOffset = 30;
	int graphDX = 360;
	int graphDY = 140;
	int graphY = 0;
	int gapY = 30;
	int gapX = 15;
	int barY = 60;
	int lineEnter = 15;
	int barDX = 200;
	int bar_p = 1;
	int titleY = 2;
	
	int plotH;
	int plotW;
};

