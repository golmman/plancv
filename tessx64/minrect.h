#ifndef _MINRECT_H_
#define _MINRECT_H_

#include "common.h"
#include "lineseg.h"

// == 0 if vectors are colinear
//  < 0 if clockwise
//  > 0 if counter-clockwise
inline int64 orientation(const Point& a, const Point& b, const Point& c) {
	//cout << "-------" << a << " " << b << " " << c << endl;
	//cout << "-------  " << (b[0] - a[0]) * (c[1] - b[1]) - (b[1] - a[1]) * (c[0] - b[0]) << endl;
	return (int64)(b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
}


inline int64 sign(int64 x) {
	return (x > 0) - (x < 0);
}

inline int sign1(int64 x) {
	return x == 0 ? 1 : (x < 0 ? 2 : 1);
}

int convex_hull4(const Point** points);
void min_linesegw(LineSegW& lw, const LineSeg& l0, const LineSeg& l1);
void min_linesegw(LineSegW& lw, const LineSegW& l0, const LineSegW& l1);

void test_convex_hull4();
void test_min_linesegw();





typedef int64 coord_t;   // coordinate type
typedef int64 coord2_t;  // must be big enough to hold 2*max(|coordinate|)^2

struct Point_TEST {
	coord_t x, y;

	bool operator <(const Point_TEST &p) const {
		return x < p.x || (x == p.x && y < p.y);
	}
};

vector<Point_TEST> convex_hull(vector<Point_TEST> P);





#endif