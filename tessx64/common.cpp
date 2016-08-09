#include "common.h"



clock_t timer = (clock_t)0;
clock_t timer_start = clock();


clock_t progress_msg(const char* msg) {
	clock_t timer_old = timer;
	timer = clock();

	if (timer_old != 0) {
		cout << " " << double(timer - timer_old) / CLOCKS_PER_SEC << "s" << endl;
	}

	cout << msg << " ";

	return timer;
}

void progress_end() {
	cout << " " << double(clock() - timer) / CLOCKS_PER_SEC << "s" << endl;
	cout << "TOTAL TIME: " << double(clock() - timer_start) / CLOCKS_PER_SEC << "s" << endl;
}


// see C:\Program Files (x86)\OpenCV\opencv\sources\modules\core\src\drawing.cpp
// at line 697
void pixel_draw(cv::Mat& img, int x, int y, pcv::Color c) {
	uchar *ptr = img.data, *tptr;
	size_t step = img.step;
	cv::Size size = img.size();
	int pix_size = (int)img.elemSize();

	if(0 <= x && x < size.width && 0 <= y && y < size.height) {
		if (pix_size == 3) {
			tptr = ptr + y*step + x*3;  
			tptr[0] = pcv::get_b(c);        
			tptr[1] = pcv::get_g(c);        
			tptr[2] = pcv::get_r(c);
		} else if (pix_size == 1) { 
			tptr = ptr + y*step + x;  
			tptr[0] = (uint8)c;   
		} else {
			assert(0);
		}
	}
}

pcv::Color pixel_read(const cv::Mat& img, int x, int y) {
	uchar *ptr = img.data, *tptr;
	size_t step = img.step;
	cv::Size size = img.size();
	int pix_size = (int)img.elemSize();

	if(0 <= x && x < size.width && 0 <= y && y < size.height) {      
		if (pix_size == 3) {
			tptr = ptr + y*step + x*3; 
			return pcv::color(tptr[0], tptr[1], tptr[2]);  
		} else if (pix_size == 1) { 
			tptr = ptr + y*step + x; 
			return pcv::color(tptr[0], tptr[0], tptr[0]);  
		}
	}

	return pcv::NO_COLOR;
}


cv::Mat image_create(int w, int h, int color_depth) {
	if (color_depth == 24) {
		return cv::Mat::zeros(h, w, CV_8UC3);
	} else if (color_depth == 8) {
		return cv::Mat::zeros(h, w, CV_8U);
	} else {
		assert(0);
	}

	return Mat();
}

void image_write(cv::InputArray img, const char* filename) {
	vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(3);
	imwrite(filename,  img, compression_params);
}

