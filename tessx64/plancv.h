#ifndef _PLANCV_H_
#define _PLANCV_H_

#include "common.h"


class PlanInfo {
public:
	char* filename;							// path and name of the input file
	int scale;								// scale used for line evaluation
	double hough_dres;						// accumulator distance resolution, accum.h ~ img.h / hough_dres
	double hough_ares;						// accumulator angle resolution, accum.w = pi / hough_ares
	int hough_thr;							// hough transform vote threshold

	int min_line_length;					// minimal desired line length
	int max_line_gap;						// maximal desired gap distance of points to form a line

	double (*peak_smoother)(int);			// smoothing function for analysed hough votes

	int cc_filter_width;					// connected components with smaller width and height are removed
	int cc_filter_height;					// connected components with smaller width and height are removed
};

class PlanChunk {
public:
	int linex1, liney1, linex2, liney2;
	double linescore;

	PlanChunk(int x1, int y1, int x2, int y2, double score) {
		linex1 = x1;
		liney1 = y1;
		linex2 = x2;
		liney2 = y2;
		linescore = score;
	}
};

class PlanData {
public:
	
	int width, height;

	PlanInfo info;

	vector<PlanChunk> chunks;
};




int plancv(PlanData& data);



#endif