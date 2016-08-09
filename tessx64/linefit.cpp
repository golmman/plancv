#include "linefit.h"


inline const Point& gimme(const Point& c) { return c; }
inline const Point& gimme(const Point* c) { return *c; }

template <typename collection_t>
void _linefit(const collection_t& points, LineSegW& line) {
	// Least Squares Fitting as described at
	// http://mathworld.wolfram.com/LeastSquaresFitting.html

	int64 x, y;
	int64 n = 0;
	int64 xsum = 0;
	int64 ysum = 0;
	int64 xsum_2 = 0;
	int64 ysum_2 = 0;
	int64 xysum = 0;

	// calculate sums
	for (auto point = points.begin(); point != points.end(); ++point) {
		x = gimme(*point).x;
		y = gimme(*point).y;
		xsum += x;
		ysum += y;
		xsum_2 += (x*x);
		ysum_2 += (y*y);
		xysum += (x*y);
		++n;
	}

	// determine variance
	int64 xvar = 0;
	int64 yvar = 0;
	int64 xmean = xsum / n;
	int64 ymean = ysum / n;
	for (auto point = points.begin(); point != points.end(); ++point) {
		x = gimme(*point).x;
		y = gimme(*point).y;
		xvar += ((x - xmean) * (x - xmean));
		yvar += ((y - ymean) * (y - ymean));
	}

	// determine line equation t maps to u + t*v
	Point2d u, v;
	if (xvar > yvar) { // avoid too steep ascent
		double b = (double)(n*xysum - xsum*ysum) / (n*xsum_2 - xsum*xsum);
		double a = (double)(ysum - b*xsum) / n;

		v.x = 1.0 / sqrt(b*b + 1.0);	// = cos(atan(b));
		v.y = b * v.x;				// = sin(atan(b));

		u.x = 0.0;
		u.y = a;
	} else {
		double b_inv = (double)(n*xysum - xsum*ysum) / (n*ysum_2 - ysum*ysum);
		double a_x = (double)(xsum - b_inv*ysum) / n;

		v.y = 1.0 / sqrt(b_inv*b_inv + 1.0);	// = sin(pi/2-atan(b_inv));
		v.x = b_inv * v.y;					// = cos(pi/2-atan(b_inv));

		u.y = 0.0;
		u.x = a_x;
	}

	// Determine length and width by projecting points to the line and its perpendicular.
	// Note that v is normalised.
	Point2d w(-v.y, v.x);
	Point2d p;
	double minv = (double)INT_MAX, maxv = (double)-INT_MAX;
	double minw = (double)INT_MAX, maxw = (double)-INT_MAX;
	double dot;
	for (auto point = points.begin(); point != points.end(); ++point) {
		p = Point2d((double)gimme(*point).x - u.x, (double)gimme(*point).y - u.y);

		dot = fast_dot(v, p);
		if (dot < minv) minv = dot;
		if (dot > maxv) maxv = dot;

		dot = fast_dot(w, p);
		if (dot < minw) minw = dot;
		if (dot > maxw) maxw = dot;
	}

	// now we have all we need to fill the line data
	line.sx = cvRound(u.x + minv * v.x);
	line.sy = cvRound(u.y + minv * v.y);
	line.ex = cvRound(u.x + maxv * v.x);
	line.ey = cvRound(u.y + maxv * v.y);
	line.width = (float)(max(1.0, max(2.0*abs(minw), 2.0*abs(maxw)))); // actually one could just search for the largest abs value...
	line.length = (float)(maxv - minv);
}

void line_fit(const vector<Point>& points, LineSegW& line) {
	_linefit(points, line);
}

void line_fit(const vector<Point*>& points, LineSegW& line) {
	_linefit(points, line);
}

void line_fit(const forward_list<Point>& points, LineSegW& line) {
	_linefit(points, line);
}

void line_fit(const forward_list<Point*>& points, LineSegW& line) {
	_linefit(points, line);
}



void test_linefit(int samples) {
	const int S = 255;

	for (int k = 0; k < samples; ++k) {
		char c = (char)k + 65;
		std::string s(&c, 1);

		cout << "line: " << c << endl;

		LineSegW l0(rand()%S, rand()%S, rand()%S, rand()%S, 1);
		LineSegW l1(rand()%S, rand()%S, rand()%S, rand()%S, 1);
		LineSegW minrect;

		cout << l0 << endl;
		cout << l1 << endl;

		vector<Point> points;
		points.push_back(l0.s); points.push_back(l0.e);
		points.push_back(l1.s); points.push_back(l1.e);

		line_fit(points, minrect);

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
		fillConvexPoly(img, poly, 4, cv::Scalar(32, 128, 32));
		line(img, l0.s, l0.e, cv::Scalar(0, 0, 255));
		line(img, l1.s, l1.e, cv::Scalar(0, 0, 255));



		imshow(s, img);
	}

	cv::waitKey();
}