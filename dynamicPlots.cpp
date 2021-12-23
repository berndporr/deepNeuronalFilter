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
    frame = _frame;
    graphY = graphDY + gapY;
}
dynaPlots::~dynaPlots() = default;

void dynaPlots::plotMainSignals(std::vector<double> outer_raw,
				std::vector<double> outer,
				std::vector<double> outer_end,
                                std::vector<double> inner_raw,
				std::vector<double> inner,
                                std::vector<double> remover,
				std::vector<double> fnn,
                                std::vector<double> l1_plot,
				const std::vector<double>& l2_plot,
				const std::vector<double>& l3_plot,
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
    double l1_min = *min_element(l1_plot.begin(), l1_plot.end());
    double l1_max = *max_element(l1_plot.begin(), l1_plot.end());

    int step = 0;
    cvui::sparkline(frame, outer_raw, graphX, graphY * step + topOffset, graphDX, graphDY, 0x000000); //black
    cvui::sparkline(frame, outer,     graphX, graphY * step + topOffset, graphDX, graphDY, 0xffffff); //white
    cvui::sparkline(frame, outer_end,     graphX, graphY * step + topOffset, graphDX, graphDY, 0x7d7d7d); //gray
    cvui::text(     frame,            graphX, graphY * step + topOffset + graphDY, "Outer: raw(b) & filtered(w) & end(gray)");
    cvui::printf(   frame,            graphX, graphY * step + topOffset + graphDY + lineEnter, "min: %+.5lf max: %+.5lf value: %+.5lf", outer_min, outer_max, outer_v);
    step ++;
    cvui::sparkline(frame, inner_raw, graphX, graphY * step + topOffset, graphDX, graphDY, 0x000000); //black
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

void dynaPlots::plotVariables(){
	int graphX = gapX;
	int barX = graphDX + graphX + gapX;

	int step = 0;
	cvui::text(    frame, barX + 60, barY * (bar_p + step) + topOffset, "outer gain");
	cvui::trackbar(frame, barX,      barY * (bar_p + step) + topOffset, barDX, &outer_gain,  (double)gainStart, (double)gainEnd);
	step ++;
	cvui::text(    frame, barX + 60, barY * (bar_p + step) + topOffset, "inner gain");
	cvui::trackbar(frame, barX,      barY * (bar_p + step) + topOffset, barDX, &inner_gain,  (double)gainStart, (double)gainEnd);
	step ++;
	cvui::text(    frame, barX + 60, barY * (bar_p + step) + topOffset, "remover gain");
	cvui::trackbar(frame, barX,      barY * (bar_p + step) + topOffset, barDX, &remover_gain, (double)gainStart, (double)gainEnd);
	step ++;
	cvui::text(    frame, barX + 60, barY * (bar_p + step) + topOffset, "feedback gain");
	cvui::trackbar(frame, barX,      barY * (bar_p + step) + topOffset, barDX, &feedback_gain,  (double)gainStart, (double)gainEnd);
	step ++;
	cvui::text(    frame, barX + 60, barY * (bar_p + step) + topOffset, "weight Eta");
	cvui::trackbar(frame, barX,      barY * (bar_p + step) + topOffset, barDX, &wEta,       (double)1.,   (double)10.);
	step ++;
	cvui::text(    frame, barX + 60, barY * (bar_p + step) + topOffset, "w power");
	cvui::trackbar(frame, barX,      barY * (bar_p + step) + topOffset, barDX, &wEtaPower,  (double)-3.,  (double)3.);
	step ++;
	cvui::text(    frame, barX + 60, barY * (bar_p + step) + topOffset, "bias Eta");
	cvui::trackbar(frame, barX,      barY * (bar_p + step) + topOffset, barDX, &bEta,       (double)1.,   (double)10.);
	step ++;
	cvui::text(    frame, barX + 60, barY * (bar_p + step) + topOffset, "b power");
	cvui::trackbar(frame, barX,      barY * (bar_p + step) + topOffset, barDX, &bEtaPower,  (double)-3.,  (double)3.);

}

void dynaPlots::plotTitle(std::string title, long count, int duration){
    cvui::printf(frame, gapX,           titleY, "Subject %s -- Sample number: %ld , Duration: %d [min] %d [s]",
                 title.c_str(),count, int(duration/60) , duration % 60);
}
