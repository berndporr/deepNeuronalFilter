#include <iostream>
#include <fstream>
#include <stdio.h>
#include <boost/circular_buffer.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <Iir.h>
#include <Fir1.h>
#include <memory>
#include "dynamicPlots.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <numeric>
#include <string>
#include <functional>
#include <assert.h>
#include <ctgmath>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <math.h>

#include "cvui.h"

#include <initializer_list>

//#define CVUI_IMPLEMENTATION
//#include "cvui.h"


using namespace std;
namespace cv {
    class Mat;
}

dynaPlots::dynaPlots(cv::Mat &_frame, int _plotW, int _plotH) {
    plotH = _plotH;
    plotW = _plotW;
    graphDX = plotW - gapX * 2;
    frame = _frame;
    graphY = graphDY + gapY;
}
dynaPlots::~dynaPlots() = default;

void dynaPlots::plotMainSignals(std::vector<double> outer,
				std::vector<double> inner,
                                std::vector<double> remover,
				std::vector<double> fnn,
                                std::vector<double> lms_output,
                                int _positionOPEN){

    int graphX = gapX;

    double outer_min = *min_element(outer.begin(), outer.end());
    double outer_max = *max_element(outer.begin(), outer.end());
    double outer_v = *outer.end();
    double inner_min = *min_element(inner.begin(), inner.end());
    double inner_max = *max_element(inner.begin(), inner.end());
    double inner_v = *inner.end();
    double remover_min = *min_element(remover.begin(), remover.end());
    double remover_max = *max_element(remover.begin(), remover.end());
    double remover_v = *remover.end();
    double fnn_min = *min_element(fnn.begin(), fnn.end());
    double fnn_max = *max_element(fnn.begin(), fnn.end());
    double fnn_v = *fnn.end();

    int step = 0;
    cvui::sparkline(frame, outer,     graphX, graphY * step + topOffset, graphDX, graphDY, 0xffffff); //white
    cvui::text(     frame,            graphX, graphY * step + topOffset + graphDY, "Outer: raw(b) & filtered(w) & end(gray)");
    cvui::printf(   frame,            graphX, graphY * step + topOffset + graphDY + lineEnter, "min: %+.5lf max: %+.5lf value: %+.5lf", outer_min, outer_max, outer_v);
    step ++;
    cvui::sparkline(frame, inner,     graphX, graphY * step + topOffset, graphDX, graphDY, 0xffffff); //white
    cvui::text(     frame,            graphX, graphY * step + topOffset + graphDY, "inner: raw(b) & filtered(w)");
    cvui::printf(   frame,            graphX, graphY * step + topOffset + graphDY + lineEnter, "min: %+.5lf max: %+.5lf value: %+.5lf", inner_min, inner_max, inner_v);
    step ++;
    cvui::sparkline(frame, remover,   graphX, graphY * step + topOffset, graphDX, graphDY, 0xffffff); //white
    cvui::text(     frame,            graphX, graphY * step + topOffset + graphDY, "remover");
    cvui::printf(   frame,            graphX, graphY * step + topOffset + graphDY + lineEnter, "min: %+.5lf max: %+.5lf value: %+.5lf", remover_min, remover_max, remover_v);
    step ++;
    cvui::sparkline(frame, fnn,       graphX, graphY * step + topOffset, graphDX, graphDY, 0xffffff); //white
    cvui::text(     frame,            graphX, graphY * step + topOffset + graphDY, "DNF output");
    cvui::printf(   frame,            graphX, graphY * step + topOffset + graphDY + lineEnter, "min: %+.5lf max: %+.5lf value: %+.5lf", fnn_min, fnn_max, fnn_v);
    step ++;
    cvui::sparkline(frame, lms_output, graphX, graphY * step + topOffset, graphDX, graphDY, 0xffffff); //white
    cvui::text(     frame,            graphX, graphY * step + topOffset + graphDY, "LMS output");
}

void dynaPlots::plotTitle(std::string title, long count, int duration){
    cvui::printf(frame, gapX,           titleY, "Subject %s -- Sample number: %ld , Duration: %d [min] %d [s]",
                 title.c_str(),count, int(duration/60) , duration % 60);
}
