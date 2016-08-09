#include "imgcc.h"


// Finds the 8-connected components of the input image.
// Note that the input image must be grayscale and all pixels are set to zero afterwards.
void cc_find(Mat& img, vector<vector<Point>>& components) {
	
	assert(img.elemSize() == 1);

	uint8 *imgptr = img.data;
	int step = (int)img.step;

	vector<Point> stack;

	#define  READ_PIXEL(x, y) (imgptr[(y)*step + (x)])
	#define  CLEAR_PIXEL(x, y) (imgptr[(y)*step + (x)] = 0)

	for (int imgy = 1; imgy < img.rows-1; ++imgy) {
		for (int imgx = 1; imgx < img.cols-1; ++imgx) {
			
			if (READ_PIXEL(imgx, imgy)) {
				// We found an untouched pixel so collect all pixels connected to it.
				components.push_back(vector<Point>());
				vector<Point>& comp = components.back();

				// perform flood fill
				stack.push_back(Point(imgx, imgy));

				while(!stack.empty()) {
					int x = stack.back().x;
					int y = stack.back().y;
					stack.pop_back();

					if (READ_PIXEL(x, y)) {
						stack.push_back(Point(x-1, y-1));
						stack.push_back(Point(x-0, y-1));
						stack.push_back(Point(x+1, y-1));

						stack.push_back(Point(x-1, y-0));
						stack.push_back(Point(x+1, y-0));

						stack.push_back(Point(x-1, y+1));
						stack.push_back(Point(x-0, y+1));
						stack.push_back(Point(x+1, y+1));

						comp.push_back(Point(x, y));

						CLEAR_PIXEL(x, y);
					}

					
				}
			}

		}
	}

	#undef CLEAR_PIXEL
	#undef READ_PIXEL
}

// Removes 8-connected components in an image which have specified width and height ranges.
void cc_clear(Mat& img, int min_width, int max_width, int min_height, int max_height) {
	Mat img_clone = img.clone();
	vector<vector<Point>> components;

	cc_find(img_clone, components);

	for (int k = 0; k < components.size(); ++k) {
		cv::Rect rect = cv::boundingRect(components[k]);

		if (min_width <= rect.width && rect.width <= max_width &&
			min_height <= rect.height && rect.height <= max_height) 
		{
			for (int j = 0; j < components[k].size(); ++j) {
				pixel_draw(img, components[k][j].x, components[k][j].y, 0);
			}
		}
	}
}

void cc_clear(Mat& img, int min_area, int max_area) {
	
}


void test_scope() {
	vector<vector<int>> vv;

	// v is filled and then in its entirety copied to vv, ok but slow
	if (clock() < 1000000) {
		vector<int> v;
		v.push_back(1); v.push_back(10); v.push_back(100);
		vv.push_back(v);
	}

	// the empty vector v is copied to vv, then the copy at vv is expanded, ok and fast
	if (clock() < 1000000) {
		vector<int> v;
		vv.push_back(v);
		vv.back().push_back(5); vv.back().push_back(4); vv.back().push_back(3);
	}

	// the empty vector v is copied to vv, then v is filled and gets destroyed, FAILS
	if (clock() < 1000000) {
		vector<int> v;
		vv.push_back(v);
		v.push_back(241); v.push_back(242); v.push_back(243);

	}

	for (int k = 0; k < vv.size(); ++k) {
		cout << vv[k] << endl;
	}
}


void test_imgcc() {

	Mat img_src = imread("test_plan5.png", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_thr, img_thr2, img_out;
	threshold(img_src, img_thr, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
	
	img_thr2 = img_thr.clone();
	cvtColor(img_thr, img_out, CV_GRAY2BGR);

	progress_msg("imgcc...");
	vector<vector<Point>> components;
	cc_find(img_thr, components);

	progress_msg("cc drawing...");
	for (int k = 0; k < components.size(); ++k) {
		//cv::rectangle(img_out, cv::boundingRect(components[k]), Scalar(0, 0, 255));
		Scalar color(rand() % 256, rand() % 256, rand() % 256);
		for (int j = 0; j < components[k].size(); ++j) {
			pixel_draw(img_out, components[k][j].x, components[k][j].y, pcv::to_color(color));
		}
	}
	image_write(img_out, "debug_imgcc.png");

	progress_msg("cc_clear...");
	cc_clear(img_thr2, 0, 50, 0, 50);
	image_write(img_thr2, "debug_clear_cc.png");

	progress_end();

	
}