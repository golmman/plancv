#ifndef _COMMON_H_
#define _COMMON_H_

//#define NDEBUG

#include <assert.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <forward_list>
#include <list>
#include <time.h>
#include <limits>

#pragma warning(push, 0)	// disable warnings in external libraries
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#pragma warning(pop)

#ifndef NDEBUG
	#define PCV_DEBUG(x) x
#else
	#define PCV_DEBUG(x) ((void)0)
#endif




using std::cout;
using std::endl;
using std::vector;
using std::forward_list;
using std::list;
using std::min;
using std::max;

using tesseract::TessBaseAPI;

using cv::Vec2i;
using cv::Vec2d;
using cv::Vec2f;
using cv::Vec4i;
using cv::Mat;
using cv::Point;
using cv::Point2f;
using cv::Point2d;
using cv::Scalar;
using cv::InputArray;
using cv::OutputArray;

using cv::imread;
using cv::imwrite;
using cv::line;


typedef signed char             int8;
typedef unsigned char           uint8;
typedef short                   int16;
typedef unsigned short          uint16;
typedef int                     int32;
typedef unsigned int            uint32;
typedef long long				int64;	
typedef unsigned long long		uint64;



namespace pcv {
	typedef uint32 Color;

	const Color NO_COLOR = 0xFF000000;

	inline Color color(uint8 b, uint8 g, uint8 r) {
		return (b << 16) | (g << 8) | (r << 0);
	}

	inline uint8 get_b(Color c) {
		return (uint8)((c >> 16) & 0xFF);
	}
	inline uint8 get_g(Color c) {
		return (uint8)((c >> 8) & 0xFF);
	}
	inline uint8 get_r(Color c) {
		return (uint8)((c >> 0) & 0xFF);
	}

	inline Scalar to_scalar(Color c) {
		return Scalar((double)get_b(c), (double)get_g(c), (double)get_r(c));
	}
	inline Color to_color(const Scalar& s) {
		return color((uint8)s[0], (uint8)s[1], (uint8)s[2]);
	}
}




template<typename _Tp>
inline double fast_norm(const cv::Point_<_Tp>& p) {
	return sqrt(p.x * p.x + p.y * p.y);
}

template<typename _Tp>
inline double fast_norm(_Tp x, _Tp y) {
	return sqrt(x * x + y * y);
}

template<typename _Tp>
inline _Tp fast_dot(const cv::Point_<_Tp>& p0, const cv::Point_<_Tp>& p1) {
	return p0.x * p1.x + p0.y * p1.y;
}

template<typename _Tp>
inline double fast_dist(const cv::Point_<_Tp>& p0, const cv::Point_<_Tp>& p1) {
	_Tp dx = p1.x - p0.x;
	_Tp dy = p1.y - p0.y;
	return sqrt(dx * dx + dy * dy);
}

template<typename _Tp>
inline double fast_dist(_Tp x0, _Tp y0, _Tp x1, _Tp y1) {
	_Tp dx = x1 - x0;
	_Tp dy = y1 - y0;
	return sqrt(dx * dx + dy * dy);
}

template<typename _Tp>
inline cv::Point_<_Tp>& operator/=(cv::Point_<_Tp>& p, _Tp t) {
	p.x /= t;
	p.y /= t;
	return p;
}

inline cv::Point_<float>& operator/=(cv::Point_<float>& p, double t) {
	p.x /= (float)t;
	p.y /= (float)t;
	return p;
}

//template<typename _Tp>
//inline cv::Point_<_Tp>& operator*=(cv::Point_<_Tp>& p, _Tp t) {
//	p.x *= t;
//	p.y *= t;
//	return p;
//}
//template<typename _Tp>
//inline cv::Point_<_Tp> operator+(const cv::Point_<_Tp>& p0, const cv::Point_<_Tp>& p1) {
//	return Point(p0.x + p1.x, p0.y + p1.y);
//}
//template<typename _Tp>
//inline cv::Point_<_Tp> operator-(const cv::Point_<_Tp>& p0, const cv::Point_<_Tp>& p1) {
//	return Point(p0.x + p1.x, p0.y + p1.y);
//}


// swaps last element and element at index then erases the last element
template <class T> 
inline void fast_erase(vector<T>& vec, int index) {
  T c(vec[index]); vec[index] = vec.back(); vec.back() = c;
  vec.pop_back();
}


template <typename T>
std::ostream& operator<<(std::ostream& out, const vector<T>& vec) {
	if (vec.size() == 0) {
		out << "[]";
		return out;
	}

	out << "[" << vec[0];
	for (int k = 1; k < vec.size(); ++k) {
		out << ", " << vec[k];
	}
	out << "]";

	return out;
}

void pixel_draw(cv::Mat& img, int x, int y, pcv::Color c);
pcv::Color pixel_read(const cv::Mat& img, int x, int y);
void image_write(cv::InputArray img, const char* filename);
cv::Mat image_create(int w, int h, int color_depth = 24);
clock_t progress_msg(const char* msg);
void progress_end();

#endif