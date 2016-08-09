#ifndef _IMGCC_H_
#define _IMGCC_H_
// image connected components

#include "common.h"




void cc_find(Mat& img, vector<vector<Point>>& components);
void cc_clear(Mat& img, int min_width, int max_width, int min_height, int max_height);

void test_imgcc();


#endif