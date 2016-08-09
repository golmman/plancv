#ifndef _CONNECTLINES_H_
#define _CONNECTLINES_H_

#include "common.h"

#include "lineseg.h"



double angle2d(const Vec2d& v);
double angle_line(const Vec2d& v);
void getAandD(double& angle, double& distance, const Vec4i& line);
double dist_lp(const Vec2d& u, const Vec2d& v, int x, int y);
double dist_ll(const LineSeg& l1, const Vec2d& u1, const Vec2d& v1, const LineSeg& l2, const Vec2d& u2, const Vec2d& v2);
void lines_connect(vector<LineSeg>& lines, int max_gap, double max_dist);
void linesw_connect(vector<LineSeg>& lines, vector<LineSegW>& linesw,  int max_gap, double max_dist);
void linesw_connect(vector<LineSegW>& linesw_in, vector<LineSegW>& linesw_out,  int max_gap);


#endif