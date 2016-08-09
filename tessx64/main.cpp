/*

	Construction/Floor Plan Computer Vision (PCV)

	Code conventions:
	class CamelCase {};
	const int CONSTANTS = 0;
	void methods_all_minuscles_and_underscore_between_words();
	int variables_same_as_methods;
	void method_argument_order(input, output, par0, par1, ...);
	variable numerating beginning with zero: int i0, i1;
	unit testing methods: void test_<filename>();
	method naming: void whatobject_howmodified(); similar to OOP e.g. Line.draw();
		examples: image_write, lines_connect, pixel_draw


	Preprocessor defines:
	https://msdn.microsoft.com/en-us/library/hhzbb5c8(v=vs.110).aspx

*/

#define _USE_MATH_DEFINES

#include "main.h"

#include "hillclimb.h"
#include "simpleedge.h"
#include "bresenham.h"
#include "plancv.h"
#include "connectlines.h"
#include "smooth_angle.h"
#include "minrect.h"
#include "lineseg.h"
#include "linefit.h"
#include "text.h"
#include "imgcc.h"
#include "graph.h"
#include "quadtree.h"
#include "raster.h"
#include "corners_connect.h"




void init() {
	PCV_DEBUG(cout << "debug mode is active" << endl);

	srand((unsigned int)time(NULL));
}


int main() {
	init();

	//test_raster();

	//test_corners_connect();

	//test_quadtree();

	//test_graph();

	//test_imgcc();

	//test_convex_hull4();

	//test_min_linesegw();

	//test_linefit(1);
	
	//system("pause");
	//return 0;

	//vector<double> partition;
	//vector<double> sm;
	//sm.push_back(0.0);
	//sm.push_back(0.5);
	//sm.push_back(0.0);
	//sm.push_back(0.5);
	//sm.push_back(0.0);
	//sm.push_back(0.25);
	//sm.push_back(0.0);

	//partition_weighted_normed(partition, sm);

	//for (int k = 0; k < partition.size(); ++k) {
	//	cout << partition[k] << endl;
	//}

	//system("pause");
	//return 0;

	//bhm_benchmark(bhm_plot3, 1000000);
	//bhm_validity(bhm_plot3);

	//Vec2d u(2, 2);
	//Vec2d v(-1, 1);

	//cout << dist_lp(u, v, 0, 1) << endl;

	//cv::Vec4i v(1, 2, 3, 4);

	//LineSeg& v2 = (LineSeg&)v;
	//v2.ey = 5;

	//cv::Vec2i p = v2.s + v2.e;

	//cout << v << endl;
	//cout << v2.s.val[0] << " "  << v2.s.val[1] << " "  << v2.e.val[0] << " "  << v2.e.val[1] << endl;
	//cout << v2.sx << " "  << v2.sy << " "  << v2.ex << " "  << v2.ey << endl;
	//cout << v2[0] << " "  << v2[1] << " "  << v2[2] << " "  << v2[3] << endl;
	//cout << v2 << endl;
	//cout << p[0] << " " << p[1] << endl;

	//v2.e = p;
	//cout << v << endl;

	//system("pause");
	//return 0;



	//double in[] = {2.0, 2.0};
	//double step[] = {0.71, 0.67};

	//hillclimb(test_hill, in, step, 2);

	//system("pause");
	//return 0;


	

	PlanData pd;
	pd.info.filename = "test_plan0.png";
	pd.info.scale = 40;
	pd.info.hough_dres = 1;			// 0.125
	pd.info.hough_ares = CV_PI/256; 	// 2048
	pd.info.hough_thr = 200;			// 50
	pd.info.min_line_length = 30;
	pd.info.max_line_gap = 5;
	pd.info.peak_smoother = sqrt;
	pd.info.cc_filter_width = 150;
	pd.info.cc_filter_height = 150;

	plancv(pd);




	progress_end();

	system("pause");
	//cout << "press any key to exit" << endl;
	//waitKey();

	return 0;
}