#include "minrect.h"


const char N = 0;
const char A = 1;
const char B = 2;
const char C = 3;
const char D = 4;

// lookup table used in convex_hull4
const char hull4_table[] = {		
							// abc abd cad bcd
	A, B, C, N,				// ++++		D inside
	A, B, C, N,				// +++0		D on edge BD
	A, B, D, C,				// +++-

	A, B, C, N,				// ++0+
	A, B, C, N,				// ++00		D == C
	A, B, D, N,				// ++0-

	A, B, C, D,				// ++-+
	A, B, D, N,				// ++-0
	A, B, D, N,				// ++--

							// abc abd cad bcd

	A, B, C, N,				// +0++
	A, B, C, N,				// +0+0		B == D
	A, D, C, N,				// +0+-

	A, B, C, N,				// +00+		A == D
	N, N, N, N,				// +000		impossible
	N, N, N, N,				// +00-		impossible

	B, C, D, N,				// +0-+
	N, N, N, N,				// +0-0		impossible
	N, N, N, N,				// +0--		impossible

							// abc abd cad bcd

	A, D, B, C,				// +-++
	A, D, C, N,				// +-+0
	A, D, C, N,				// +-+-

	B, C, D, N,				// +-0+
	N, N, N, N,				// +-00		impossible
	N, N, N, N,				// +-0-		impossible

	B, C, D, N,				// +--+
	N, N, N, N,				// +--0		impossible
	N, N, N, N,				// +---		impossible

	////////////////////////// abc abd cad bcd

	N, N, N, N,				// 0+++		impossible
	N, N, N, N,				// 0++0		impossible
	B, D, C, N,				// 0++-

	N, N, N, N,				// 0+0+		impossible
	N, N, N, N,				// 0+00		impossible
	A, B, D, N,				// 0+0-		A == C

	A, C, D, N,				// 0+-+
	A, B, D, N,				// 0+-0		B == C
	A, B, D, N,				// 0+--

							// abc abd cad bcd

	N, N, N, N,				// 00++		impossible
	N, N, N, N,				// 00+0		impossible
	A, D, C, N,				// 00+-		A == B

	N, N, N, N,				// 000+		impossible
	N, N, N, N,				// 0000		UNDEFINED
	N, N, N, N,				// 000-		impossible

	A, C, D, N,				// 00-+		A == B
	N, N, N, N,				// 00-0		impossible
	N, N, N, N,				// 00--		impossible

							// abc abd cad bcd

	A, D, B, N,				// 0-++
	A, D, B, N,				// 0-+0		B == C
	A, D, C, N,				// 0-+-

	A, D, B, N,				// 0-0+		A == C
	N, N, N, N,				// 0-00		impossible
	N, N, N, N,				// 0-0-		impossible

	B, C, D, N,				// 0--+
	N, N, N, N,				// 0--0		impossible
	N, N, N, N,				// 0---		impossible

	////////////////////////// abc abd cad bcd

	N, N, N, N,				// -+++		impossible
	N, N, N, N,				// -++0		impossible
	B, D, C, N,				// -++-

	N, N, N, N,				// -+0+		impossible
	N, N, N, N,				// -+00		impossible
	B, D, C, N,				// -+0-

	A, C, D, N,				// -+-+
	A, C, D, N,				// -+-0	
	A, C, B, D,				// -+--

							// abc abd cad bcd

	N, N, N, N,				// -0++		impossible
	N, N, N, N,				// -0+0		impossible
	B, D, C, N,				// -0+-

	N, N, N, N,				// -00+		impossible
	N, N, N, N,				// -000		impossible
	A, C, B, N,				// -00-		A == D

	A, C, D, N,				// -0-+
	A, C, B, N,				// -0-0		B == D
	A, C, B, N,				// -0--

							// abc abd cad bcd

	A, D, B, N,				// --++
	A, D, B, N,				// --+0
	A, D, C, B,				// --+-

	A, D, B, N,				// --0+
	A, C, B, N,				// --00		C == D
	A, C, B, N,				// --0-

	A, C, D, B,				// ---+
	A, C, B, N,				// ---0
	A, C, B, N,				// ----
};



// Turns the input array of 4 points to an array with the convex hull in ccw order (for a coordinate system
// where y points to the top and x to the right).
// Returns the number of points in the hull.
int convex_hull4(const Point** points) {

	const Point* p[5] = {(Point*)0, points[0], points[1], points[2], points[3]};

	char abc = (char)1 - (char)sign(orientation(*points[0], *points[1], *points[2])); // 0 = ccw(+), 1 = colinear(0), 2 = cw(-)
	char abd = (char)1 - (char)sign(orientation(*points[0], *points[1], *points[3]));
	char cad = (char)1 - (char)sign(orientation(*points[2], *points[0], *points[3]));
	char bcd = (char)1 - (char)sign(orientation(*points[1], *points[2], *points[3]));

	const char* t = hull4_table + (int)4*(bcd + 3*cad + 9*abd + 27*abc);

	if (t[0] == 0) {
		// all points colinear
		int minc = INT_MAX, maxc = -INT_MAX;
		const Point* p0;
		const Point* p1;

		// find min and max point by searching the x-coordinates
		for (int k = 0; k < 4; ++k) {
			if (minc > points[k]->x) {
				minc = points[k]->x;
				p0 = points[k];
			}
			if (maxc < points[k]->x) {
				maxc = points[k]->x;
				p1 = points[k];
			}
		}

		if (minc == maxc) {
			// all points have the same x coordinate...
			// find min and max point by searching the y-coordinates
			minc = INT_MAX;
			maxc = -INT_MAX;
			for (int k = 0; k < 4; ++k) {
				if (minc > points[k]->y) {
					minc = points[k]->y;
					p0 = points[k];
				}
				if (maxc < points[k]->y) {
					maxc = points[k]->y;
					p1 = points[k];
				}
			}

			if (minc == maxc) {
				// all points are equal...
				return 1;
			}
		}

		points[0] = p0;
		points[1] = p1;
		return 2;
	}

	points[0] = p[t[0]];
	points[1] = p[t[1]];
	points[2] = p[t[2]];
	points[3] = p[t[3]];

	return t[3] == 0 ? 3 : 4;
}


// Calculates the maximum distance from the given edge of a convex hull to the remaining point(s).
// The points are expected to be ordererd, i.e. it always calculates based on the edge given by p[0] and p[1].
double dist_ep4(const Point* p0, const Point* p1, const Point* p2) {
	int64 dx = p1->x - p0->x;
	int64 dy = p1->y - p0->y;
	int64 a = p1->x * p0->y - p1->y * p0->x;
	return (double)abs(dy * p2->x - dx * p2->y + a) / sqrt(dx * dx + dy * dy);
}

// Calculates the maximum distance from the given edge of a convex hull to the remaining point(s).
// The points are expected to be ordererd, i.e. it always calculates based on the edge given by p[0] and p[1].
double dist_ep4(const Point* p0, const Point* p1, const Point* p2, const Point* p3) {
	int64 dx = p1->x - p0->x;
	int64 dy = p1->y - p0->y;
	int64 a = p1->x * p0->y - p1->y * p0->x;
	double norm = sqrt(dx * dx + dy * dy);

	double w0 = (double)abs(dy * p2->x - dx * p2->y + a) / norm;
	double w1 = (double)abs(dy * p3->x - dx * p3->y + a) / norm;

	return max(w0, w1);
}


// Calculates the width-minimal LineSegW containing the line segments l0 and l1.
void min_linesegw(LineSegW& lw, const LineSeg& l0, const LineSeg& l1) {
	const Point* hull[4] = {&l0.s, &l0.e, &l1.s, &l1.e};

	int hullsize = convex_hull4(hull);

	if (hullsize == 1) {
		lw.s = *hull[0];
		lw.e = *hull[0];
		lw.width = 0.0f;
		lw.length = 0.0f;

	} else if (hullsize == 2) { // line points are colinear
		lw.s = *hull[0];
		lw.e = *hull[1];
		lw.width = 1.0;
		lw.length = (float)fast_dist(lw.s, lw.e);

	} else if (hullsize == 3) {

		double dist0 = dist_ep4(hull[0], hull[1], hull[2]);
		double dist1 = dist_ep4(hull[1], hull[2], hull[0]);
		double dist2 = dist_ep4(hull[2], hull[0], hull[1]);

		double mind = min(min(dist0, dist1), dist2);

		// the vertex with the closest distance to an edge is always projected on the line segment of that edge
		// proof?

		const Point* p0;
		const Point* p1;
		if (mind == dist0) {
			p0 = hull[0]; p1 = hull[1];
		} else if (mind == dist1) {
			p0 = hull[1]; p1 = hull[2];
		} else if (mind == dist2) {
			p0 = hull[2]; p1 = hull[0];
		} else {
			assert(0);
		}

		Point2f perp(-(float)(p1->y - p0->y), (float)(p1->x - p0->x));
		double norm = sqrt(perp.x * perp.x + perp.y * perp.y);
		perp *= (float)(0.5 * mind / norm);
		
		lw.s = Point(	p0->x + cvRound(perp.x), 
						p0->y + cvRound(perp.y));
		lw.e = Point(	p1->x + cvRound(perp.x), 
						p1->y + cvRound(perp.y));
		lw.width = (float)mind;
		lw.length = (float)norm;

	} else { // hullsize  == 4
		assert(hullsize  == 4);
		
		double dist0 = dist_ep4(hull[0], hull[1], hull[2], hull[3]);
		double dist1 = dist_ep4(hull[1], hull[2], hull[3], hull[0]);
		double dist2 = dist_ep4(hull[2], hull[3], hull[0], hull[1]);
		double dist3 = dist_ep4(hull[3], hull[0], hull[1], hull[2]);

		double mind = min(min(dist0, dist1), min(dist2, dist3));

		Point2f perp;
		const Point* p0;
		const Point* p1;
		const Point* p2;
		const Point* p3;
		

		if (mind == dist0) {
			p0 = hull[0]; p1 = hull[1]; p2 = hull[2]; p3 = hull[3];
		} else if (mind == dist1) {
			p0 = hull[1]; p1 = hull[2]; p2 = hull[3]; p3 = hull[0];
		} else if (mind == dist2) {
			p0 = hull[2]; p1 = hull[3]; p2 = hull[0]; p3 = hull[1];
		} else if (mind == dist3) {
			p0 = hull[3]; p1 = hull[0]; p2 = hull[1]; p3 = hull[2];
		} else {
			assert(0);
		}


		Point2f v((float)(p1->x - p0->x), (float)(p1->y - p0->y));
		Point2f u0((float)(p2->x - p0->x), (float)(p2->y - p0->y));
		Point2f u1((float)(p3->x - p0->x), (float)(p3->y - p0->y));

		double norm = sqrt(v.x * v.x + v.y * v.y);
		v /= (float)norm;
		perp = Point2f(-v.y, v.x);
		

		double t0 = v.ddot(u0);
		double t1 = v.ddot(u1);

		double minv = min(0.0,  min(t0, t1));
		double maxv = max(norm, max(t0, t1));

		perp *= (float)(0.5 * mind);

		//cout << *p0 << " " << *p1 << endl;
		//cout << v << " " << perp << endl;
		//cout << t0 << " " << t1 << endl;

		lw.s = Point(	p0->x + cvRound(perp.x + minv * v.x),
						p0->y + cvRound(perp.y + minv * v.y));
		lw.e = Point(	p0->x + cvRound(perp.x + maxv * v.x), 
						p0->y + cvRound(perp.y + maxv * v.y));
		lw.width = (float)mind;
		lw.length = (float)(maxv - minv);
	}


}

// Calculates the width-minimal LineSegW containing the line segments l0 and l1 which itself have a width.
// NOTE that this function is a simple heuristic and should not be used for exact calculations.
void min_linesegw(LineSegW& lw, const LineSegW& l0, const LineSegW& l1) {
	min_linesegw(lw, l0.lineseg, l1.lineseg);

	float maxw = max(l0.width, l1.width);
	lw.width = max(lw.width + 0.5f * maxw, maxw);
}








// tests convex_hull4, there should be no console outputs
void test_convex_hull4() {
	const int S = 10;

	for (int l = 0; l < 10000; ++l) {
	
		vector<cv::Point> hull;
		vector<cv::Point> points;
		points.push_back(Point(rand()%S, rand()%S));
		points.push_back(Point(rand()%S, rand()%S));
		points.push_back(Point(rand()%S, rand()%S));
		points.push_back(Point(rand()%S, rand()%S));
		const Point* points2[5] = {&points[0], &points[1], &points[2], &points[3], (Point*)0};
		

		cv::convexHull(points, hull);
		int n = convex_hull4(points2);


		Point points3[4];

		for (int k = 0; k < n; ++k) {
			points3[k] = *points2[k];
		}


		int equal = 0;

		for (int k = 0; k < n; ++k) {
			//cout << points3[0] << " " << points3[1] << " " << points3[2] << " " << points3[3] << endl;
			//cout << hull[0] << " " << hull[1] << " " << hull[2] << " " << hull[3] << endl;
			//cout << endl;

			//equal = 1;
			//for (int i = 0; i < n; ++i) {
			//	if (points3[i][0] !=  hull[i][0] || points3[i][1] !=  hull[i][1]) {
			//		equal = 0;
			//		break;
			//	}
			//}


			if (memcmp(&points3[0], &hull[0], n * sizeof(Point)) == 0) {
				equal = 1;
				break;
			}

			Point tmp = points3[0];
			for (int i = 0; i < n-1; ++i) {
				points3[i] = points3[i+1];
			}
			points3[n-1] = tmp;
		}

		if (equal == 1) {
			//cout << "YAY!" << endl;
		} else {
			cout << "FAIL: " << endl;
			for (int k = 0; k < 4; ++k) {
				cout << points[k] << " ";
			}
			cout << endl;

			for (int k = 0; k < n; ++k) {
				cout << points3[k] << " ";
			}
			cout << endl;
			for (int k = 0; k < hull.size(); ++k) {
				cout << hull[k] << " ";
			}
			cout << endl;
		}

	}
}



void test_min_linesegw() {
	const int S = 255;

	for (int k = 0; k < 10; ++k) {

		LineSegW l0(rand()%S, rand()%S, rand()%S, rand()%S, 1);
		LineSegW l1(rand()%S, rand()%S, rand()%S, rand()%S, 1);
		LineSegW minrect;

		cout << l0 << endl;
		cout << l1 << endl;

		min_linesegw(minrect, l0, l1);

		cout << minrect << endl;

		float w2 = minrect.width / 2;
		cv::Point poly[4];
		Point2f perp(-(float)(minrect.ey - minrect.sy), (float)(minrect.ex - minrect.sx));
		perp /= minrect.length;
		poly[0] = Point((int)(minrect.sx + w2 * perp.x), (int)(minrect.sy + w2 * perp.y));
		poly[1] = Point((int)(minrect.ex + w2 * perp.x), (int)(minrect.ey + w2 * perp.y));
		poly[2] = Point((int)(minrect.ex - w2 * perp.x), (int)(minrect.ey - w2 * perp.y));
		poly[3] = Point((int)(minrect.sx - w2 * perp.x), (int)(minrect.sy - w2 * perp.y));


		cv::Mat img = image_create(S, S);
		//line(img, minrect.s, minrect.e, Scalar(32, 128, 32), (int)minrect.width);
		fillConvexPoly(img, poly, 4, cv::Scalar(32, 128, 32));
		line(img, l0.s, l0.e, cv::Scalar(0, 0, 255));
		line(img, l1.s, l1.e, cv::Scalar(0, 0, 255));

		char c = (char)k + 65;
		std::string s(&c, 1);

		imshow(s, img);
	}

	cv::waitKey();
}




/* 
 * Convex hull algorithm from some wiki site.
 */



// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the points are collinear.
coord2_t cross(const Point_TEST &O, const Point_TEST &A, const Point_TEST &B)
{
	return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

// Returns a list of points on the convex hull in counter-clockwise order.
// Note: the last point in the returned list is the same as the first one.
vector<Point_TEST> convex_hull(vector<Point_TEST> P)
{
	int n = (int)P.size(), k = 0;
	vector<Point_TEST> H(2*n);

	// Sort points lexicographically
	sort(P.begin(), P.end());

	// Build lower hull
	for (int i = 0; i < n; ++i) {
		while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
		H[k++] = P[i];
	}

	// Build upper hull
	for (int i = n-2, t = k+1; i >= 0; i--) {
		while (k >= t && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
		H[k++] = P[i];
	}

	H.resize(k-1);
	return H;
}