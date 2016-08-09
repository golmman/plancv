#ifndef _BRESENHAM_H_
#define _BRESENHAM_H_

#include "common.h"
#include "lineseg.h"


struct PixelStats {
public:
	int unique1, overlap1, total1;
	int unique2, overlap2, total2;

	int minx, miny;
	vector<cv::Vec2i> overlap2_vec;
	vector<cv::Vec2i> unique2_vec;

	PixelStats() : 
		unique1(0), overlap1(0), total1(0),
		unique2(0), overlap2(0), total2(0) {}

	void draw_vectors(Mat& img) {
		for (int k = 0; k < overlap2_vec.size(); ++k) {
			line(img, 
					cv::Point(overlap2_vec[k][0]+minx, overlap2_vec[k][1]+miny), 
					cv::Point(overlap2_vec[k][0]+minx, overlap2_vec[k][1]+miny), 
					cv::Scalar(0,255,0));
		}

		for (int k = 0; k < unique2_vec.size(); ++k) {
			cv::Scalar col = cv::Scalar(255,0,0);
			for (int i = 0; i < overlap2_vec.size(); ++i) {
				if (unique2_vec[k] == overlap2_vec[i]) {
					col = cv::Scalar(255,255,0);
					break;
				}
			}

			line(img, 
				cv::Point(unique2_vec[k][0]+minx, unique2_vec[k][1]+miny), 
				cv::Point(unique2_vec[k][0]+minx, unique2_vec[k][1]+miny), 
				col);
		}
		cout << "MMMMMMMMMM " << overlap2_vec.size() << " " << unique2_vec.size() << endl;

		image_write(img, "debug_overlap_unique.png");
	}
};


void bhm_benchmark(void (*)(void (*)(int, int), int, int, int, int), int lines);
void bhm_validity(void (*)(void (*)(int, int), int, int, int, int));



void bhm_plot1(void (*)(int, int), int, int, int, int);
void bhm_plot2(void (*)(int, int), int, int, int, int);
void bhm_plot3(void (*)(int, int), int, int, int, int);
void bhm_plot4(void (*)(int, int), int, int, int, int);
static const auto& bhm_plot = bhm_plot1;	// alias for currently used line plotting algorithm




void bhm_line(Mat& mat, int x1, int y1, int const x2, int const y2, const Scalar& color);
void bhm_vote(int* const buffer, int wpl, int x1, int y1, int const x2, int const y2);
double bhm_score(int* const buffer, int wpl, int x1, int y1, int const x2, int const y2);
void bhm_hough(const Mat& edge, int min_len, int max_gap, vector<LineSeg>& lines_out, const vector<Vec2f>& lines_in);
void bhm_difference(PixelStats& stats, const vector<LineSeg>& lines1, const vector<LineSeg>& lines2);






#endif