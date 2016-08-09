#include "raster.h"

#include "common.h"

#include <opencv2\core\core.hpp>
#include <iostream>
#include <algorithm>
#include <math.h>

using std::max;
using std::min;
using std::cout;
using std::endl;
using cv::Point;




typedef pcv::Raster<float>			Ras;
typedef pcv::RasterPoint<float>		RasPoint;
typedef Ras::ItRP					ItRasPoint;

void test_raster() {

	Mat img = image_create(1000, 1000);

	Ras r(1000, 1000, 23);

	list<ItRasPoint> to_be_erased;

	for (int k = 0; k < 100000; ++k) {
		int x = rand()%1000;
		int y = rand()%1000;
		ItRasPoint rp = r.push(RasPoint(1.5f, Point(x, y)));

		if (fast_dist(Point(250, 360), *rp) < 100) {
			to_be_erased.push_back(rp);
		}
	}

	list<ItRasPoint> l;

	r.find(Point(-50, 350), 300.0, l);

	// erase some points
	for (auto val = to_be_erased.begin(); val != to_be_erased.end(); ++val) {
		r.erase(*val);
	}

	// draw the random point in the raster
	for (int k = 0; k < r.size(); ++k) {
		for (auto rrpp = r.lists[k].begin(); rrpp != r.lists[k].end(); ++rrpp) {
			pixel_draw(img, rrpp->x, rrpp->y, pcv::color(0, 0, 255));
		}
	}

	// color some green
	for (auto rasp = l.begin(); rasp != l.end(); ++rasp) {
		pixel_draw(img, (*rasp)->x, (*rasp)->y, pcv::color(0, 255, 0));
	}


	image_write(img, "debug_raster.png");
}
