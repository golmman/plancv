#ifndef _LINEFIT_H_
#define _LINEFIT_H_


#include "common.h"
#include "lineseg.h"


void line_fit(const vector<Point>& points, LineSegW& line);
void line_fit(const vector<Point*>& points, LineSegW& line);
void line_fit(const forward_list<Point>& points, LineSegW& line);
void line_fit(const forward_list<Point*>& points, LineSegW& line);

void test_linefit(int samples);

#endif