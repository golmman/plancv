#ifndef _HOUGH_H_
#define _HOUGH_H_

#include "common.h"

#include "opencv2\core\internal.hpp"


using std::vector;


void hough_transform_p_opencv(cv::InputArray _image, cv::OutputArray _lines,
                      double rho, double theta, int threshold,
                      double minLineLength, double maxGap );

void hough_transform_opencv(cv::InputArray _image, cv::OutputArray _lines,
                     double rho, double theta, int threshold,
                     double srn=0, double stn=0 );

void hough_standard(cv::InputArray _image, cv::OutputArray _lines,
                     double rho, double theta, int threshold);

void hough_analyse(cv::InputArray _image, cv::OutputArray votes, double rho, double theta, int threshold);


void hough_partition(cv::InputArray _image, cv::OutputArray _lines, double rho, const vector<double>& partition, int threshold);

#endif