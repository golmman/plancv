#include "linefit.h"


inline const Vec2i& gimme(const Vec2i& c) { return c; }
inline const Vec2i& gimme(const Vec2i* c) { return *c; }

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
		x = gimme(*point)[0];
		y = gimme(*point)[1];
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
		x = gimme(*point)[0];
		y = gimme(*point)[1];
		xvar += ((x - xmean) * (x - xmean));
		yvar += ((y - ymean) * (y - ymean));
	}

	// determine line equation t maps to u + t*v
	Vec2d u, v;
	if (xvar > yvar) { // avoid too steep ascent
		double b = (double)(n*xysum - xsum*ysum) / (n*xsum_2 - xsum*xsum);
		double a = (double)(ysum - b*xsum) / n;

		v[0] = 1.0 / sqrt(b*b + 1.0);	// = cos(atan(b));
		v[1] = b * v[0];				// = sin(atan(b));

		u[0] = 0.0;
		u[1] = a;
	} else {
		double b_inv = (double)(n*xysum - xsum*ysum) / (n*ysum_2 - ysum*ysum);
		double a_x = (double)(xsum - b_inv*ysum) / n;

		v[1] = 1.0 / sqrt(b_inv*b_inv + 1.0);	// = sin(pi/2-atan(b_inv));
		v[0] = b_inv * v[1];					// = cos(pi/2-atan(b_inv));

		u[1] = 0.0;
		u[0] = a_x;
	}

	// Determine length and width by projecting points to the line and its perpendicular.
	// Note that v is normalised.
	Vec2d w(-v[1], v[0]);
	Vec2d p;
	double minv = (double)INT_MAX, maxv = (double)-INT_MAX;
	double minw = (double)INT_MAX, maxw = (double)-INT_MAX;
	double dot;
	for (auto point = points.begin(); point != points.end(); ++point) {
		p = Vec2d((double)gimme(*point)[0] - u[0], (double)gimme(*point)[1] - u[1]);

		dot = fast_dot(v, p);
		if (dot < minv) minv = dot;
		if (dot > maxv) maxv = dot;

		dot = fast_dot(w, p);
		if (dot < minw) minw = dot;
		if (dot > maxw) maxw = dot;
	}

	// now we have all we need to fill the line data
	line.sx = cvRound(u[0] + minv * v[0]);
	line.sy = cvRound(u[1] + minv * v[1]);
	line.ex = cvRound(u[0] + maxv * v[0]);
	line.ey = cvRound(u[1] + maxv * v[1]);
	line.width = (float)(max(1.0, max(2.0*abs(minw), 2.0*abs(maxw)))); // actually one could just search for the largest abs value...
	line.length = (float)(maxv - minv);
}

void line_fit(const vector<Vec2i>& points, LineSegW& line) {
	_linefit(points, line);
}

void line_fit(const vector<Vec2i*>& points, LineSegW& line) {
	_linefit(points, line);
}

void line_fit(const forward_list<Vec2i>& points, LineSegW& line) {
	_linefit(points, line);
}

void line_fit(const forward_list<Vec2i*>& points, LineSegW& line) {
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

		vector<Vec2i> points;
		points.push_back(l0.s); points.push_back(l0.e);
		points.push_back(l1.s); points.push_back(l1.e);

		line_fit(points, minrect);

		cout << minrect << endl;

		float w2 = minrect.width / 2;
		cv::Point poly[4];
		Vec2f perp(-(float)(minrect.ey - minrect.sy), (float)(minrect.ex - minrect.sx));
		perp /= minrect.length;
		poly[0] = Vec2i((int)(minrect.sx + w2 * perp[0]), (int)(minrect.sy + w2 * perp[1]));
		poly[1] = Vec2i((int)(minrect.ex + w2 * perp[0]), (int)(minrect.ey + w2 * perp[1]));
		poly[2] = Vec2i((int)(minrect.ex - w2 * perp[0]), (int)(minrect.ey - w2 * perp[1]));
		poly[3] = Vec2i((int)(minrect.sx - w2 * perp[0]), (int)(minrect.sy - w2 * perp[1]));


		cv::Mat img = image_create(S, S);
		fillConvexPoly(img, poly, 4, cv::Scalar(32, 128, 32));
		line(img, l0.s, l0.e, cv::Scalar(0, 0, 255));
		line(img, l1.s, l1.e, cv::Scalar(0, 0, 255));



		imshow(s, img);
	}

	cv::waitKey();
}