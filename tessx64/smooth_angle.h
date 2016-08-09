#ifndef _SMOOTH_ANGLE_H_
#define _SMOOTH_ANGLE_H_

#include "common.h"

void peaks_find(const vector<int>& votes, vector<double>& peaks, double threshold);
void partition_weighted(const vector<double>& weights, vector<double>& partition);
void partition_equidistant(double theta, vector<double>& partition);
void partition_weighted_normed(const vector<double>& weights, vector<double>& partition);
void votes_normalise(const vector<int>& votes, vector<double>& weights, double (*peak_smoother)(int));
void histogram_create(const vector<double>& weights);

#endif