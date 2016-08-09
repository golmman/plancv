#ifndef _LINEFIT_H_
#define _LINEFIT_H_


#include "common.h"
#include "lineseg.h"


void line_fit(const vector<Vec2i>& points, LineSegW& line);
void line_fit(const vector<Vec2i*>& points, LineSegW& line);
void line_fit(const forward_list<Vec2i>& points, LineSegW& line);
void line_fit(const forward_list<Vec2i*>& points, LineSegW& line);

void test_linefit(int samples);

#endif