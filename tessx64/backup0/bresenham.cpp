#include "bresenham.h"

uint32 global_size = 1000;
uint32* global_buffer;
uint32 global_color = 0xFF0A00FF;

void plot_bench(int x, int y) {
	global_buffer[x + global_size * y] = global_color;
}


Mat mat_validity;
Scalar color_validity;

void plot_validity(int x, int y) {
	line(mat_validity, Point(x, y), Point(x, y), color_validity);
}

// benchmark a specific line drawing algorithm (typically bresenham variation)
void bhm_benchmark(void (*bhm)(void (*)(int, int), int, int, int, int), int lines) {
	global_buffer = new uint32[global_size * global_size];

	progress_msg("bhm_test...");
	for (int k = 0; k < lines; ++k) {
		bhm(plot_bench, rand() % global_size, rand() % global_size, rand() % global_size, rand() % global_size);
	}
	progress_msg("");

	delete[] global_buffer;
}

// test if the line drawing implementation is valid
// output image should look like (100 lines)
//
//            \ | /red
//          ___\|/___
//    colour   /|\   \green
//    gradient/ | \
//
void bhm_validity(void (*bhm)(void (*)(int, int), int, int, int, int)) {
	const int size = 1000;
	mat_validity = Mat::zeros(size, size, CV_8UC3);

	for (int k = 0; k < 100; ++k) {
		int endx = (int)(cos(2.0 * CV_PI / 100.0 * k) / 3.0 * size + size / 2);
		int endy = (int)(sin(2.0 * CV_PI / 100.0 * k) / 3.0 * size + size / 2);
		color_validity = Scalar(0, 255.0 - 255.0 / 100.0 * k, 255.0 / 100.0 * k);
		bhm(plot_validity, size / 2, size / 2, endx, endy);

		cout << k << " " << cos(2.0 * CV_PI / 100.0 * k) << endl;
	}

	image_write(mat_validity, "bhm_validity.png");
}





// from http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
void bhm_plot1(void (*plot)(int, int), int x0, int y0, int x1, int y1) {

	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
	int err = (dx>dy ? dx : -dy)/2, e2;

	for(;;){
		plot(x0,y0);
		if (x0==x1 && y0==y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}

// from http://www.edepot.com/lineex.html
// ~6.6% faster than the code from rosettacode.org
// disadvantages:	- licensed
//					- picture size limited to 65536x65536
void bhm_plot2(void (*plot)(int, int), int x, int y, int x2, int y2) {
   	bool yLonger=false;
	int shortLen=y2-y;
	int longLen=x2-x;
	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;				
		yLonger=true;
	}
	int decInc;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen>0) {
			longLen+=y;
			for (int j=0x8000+(x<<16);y<=longLen;++y) {
				plot(j >> 16,y);	
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (int j=0x8000+(x<<16);y>=longLen;--y) {
			plot(j >> 16,y);	
			j-=decInc;
		}
		return;	
	}

	if (longLen>0) {
		longLen+=x;
		for (int j=0x8000+(y<<16);x<=longLen;++x) {
			plot(x,j >> 16);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (int j=0x8000+(y<<16);x>=longLen;--x) {
		plot(x,j >> 16);
		j-=decInc;
	}

}


void bhm_plot3(void (*plot)(int, int), int x, int y, int x2, int y2) {
	bool yLonger=false;
	int incrementVal;
	int shortLen=y2-y;
	int longLen=x2-x;

	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;
		yLonger=true;
	}

	if (longLen<0) incrementVal=-1;
	else incrementVal=1;

	double multDiff;
	if (longLen==0.0) multDiff=(double)shortLen;
	else multDiff=(double)shortLen/(double)longLen;
	if (yLonger) {
		for (int i=0;i!=longLen;i+=incrementVal) {
			plot(x+(int)((double)i*multDiff),y+i);
		}
	} else {
		for (int i=0;i!=longLen;i+=incrementVal) {
			plot(x+i,y+(int)((double)i*multDiff));
		}
	}
}




// marginally slower than the rosettacode.org example
void bhm_plot4(void (*plot)(int, int), int x1, int y1, int x2, int y2) {

	int delta_x = x2 - x1;
	// if x1 == x2, then it does not matter what we set here
	signed char const ix = ((delta_x > 0) - (delta_x < 0));
	delta_x = std::abs(delta_x) << 1;

	int delta_y = y2 - y1;
	// if y1 == y2, then it does not matter what we set here
	signed char const iy = ((delta_y > 0) - (delta_y < 0));
	delta_y = std::abs(delta_y) << 1;


	plot(x1, y1);

	if (delta_x >= delta_y) {
		// error may go below zero
		int error = (delta_y - (delta_x >> 1));

		while (x1 != x2) {
			if ((error >= 0) && (error || (ix > 0))) {
				error -= delta_x;
				y1 += iy;
			}
			// else do nothing

			error += delta_y;
			x1 += ix;

			plot(x1, y1);
		}
	} else {
		// error may go below zero
		int error = (delta_x - (delta_y >> 1));

		while (y1 != y2) {
			if ((error >= 0) && (error || (iy > 0))) {
				error -= delta_y;
				x1 += ix;
			}
			// else do nothing

			error += delta_x;
			y1 += iy;

			plot(x1, y1);
		}
	}
}


// increments each pixel of a line by 1 in specified buffer
void bhm_vote(int* const buffer, int wpl, int x1, int y1, int x2, int y2) {

	static int* plot_buffer;
	static int plot_wpl;

	plot_buffer = buffer;
	plot_wpl = wpl;

	class local {
	public:
		static void plot(int x, int y) {
			plot_buffer[x + plot_wpl * y] += 1;
		}
	};

	bhm_plot(local::plot, x1, y1, x2, y2);

	return;

	//////////////////////////////////////////////// old




	int delta_x = x2 - x1;
	// if x1 == x2, then it does not matter what we set here
	signed char const ix = ((delta_x > 0) - (delta_x < 0));
	delta_x = std::abs(delta_x) << 1;

	int delta_y = y2 - y1;
	// if y1 == y2, then it does not matter what we set here
	signed char const iy = ((delta_y > 0) - (delta_y < 0));
	delta_y = std::abs(delta_y) << 1;


	//plot(x1, y1);
	buffer[x1 + wpl * y1] += 1;


	if (delta_x >= delta_y) {
		// error may go below zero
		int error = (delta_y - (delta_x >> 1));

		while (x1 != x2) {
			if ((error >= 0) && (error || (ix > 0))) {
				error -= delta_x;
				y1 += iy;
			}
			// else do nothing

			error += delta_y;
			x1 += ix;

			//plot(x1, y1);
			buffer[x1 + wpl * y1] += 1;

		}
	} else {
		// error may go below zero
		int error = (delta_x - (delta_y >> 1));

		while (y1 != y2) {
			if ((error >= 0) && (error || (iy > 0))) {
				error -= delta_y;
				x1 += ix;
			}
			// else do nothing

			error += delta_x;
			y1 += iy;

			//plot(x1, y1);
			buffer[x1 + wpl * y1] += 1;
		}
	}
}

// Reads the cumulative votes of a line from the specified buffer (as bhm_vote does),
// then return the sum of votes divided by line length.
double bhm_score(int* const buffer, int wpl, int x1, int y1, int const x2, int const y2) {

	static int* plot_buffer;
	static int plot_wpl;
	static int score;
	static int linelen;

	plot_buffer = buffer;
	plot_wpl = wpl;
	score = 0;
	linelen = 0;



	class local {
	public:
		static void plot(int x, int y) {
			score += plot_buffer[x + plot_wpl * y];
			linelen += 1;
		}
	};

	bhm_plot(local::plot, x1, y1, x2, y2);

	return (double)score / linelen;


	////////////////////////////////// old


	int delta_x = x2 - x1;
	// if x1 == x2, then it does not matter what we set here
	signed char const ix = ((delta_x > 0) - (delta_x < 0));
	delta_x = std::abs(delta_x) << 1;

	int delta_y = y2 - y1;
	// if y1 == y2, then it does not matter what we set here
	signed char const iy = ((delta_y > 0) - (delta_y < 0));
	delta_y = std::abs(delta_y) << 1;

	//int score = 0;
	//int linelen = 0;

	//plot(x1, y1);
	score += buffer[x1 + wpl * y1];
	linelen += 1;



	if (delta_x >= delta_y) {
		// error may go below zero
		int error = (delta_y - (delta_x >> 1));

		while (x1 != x2) {
			if ((error >= 0) && (error || (ix > 0))) {
				error -= delta_x;
				y1 += iy;
			}
			// else do nothing

			error += delta_y;
			x1 += ix;

			//plot(x1, y1);
			score += buffer[x1 + wpl * y1];
			linelen += 1;
		}
	} else {
		// error may go below zero
		int error = (delta_x - (delta_y >> 1));

		while (y1 != y2) {
			if ((error >= 0) && (error || (iy > 0))) {
				error -= delta_y;
				x1 += ix;
			}
			// else do nothing

			error += delta_x;
			y1 += iy;

			//plot(x1, y1);
			score += buffer[x1 + wpl * y1];
			linelen += 1;
		}
	}


	return (double)score / linelen;
}





void bhm_hough(const Mat& edge, int min_len, int max_gap, vector<LineSeg>& lines_out, const vector<Vec2f>& lines_in) {

	const static CvMat* edge_plot;
	static vector<LineSeg>* lines_out_plot;
	static int min_len_plot;
	static int max_gap_plot;
	static int len, gap, sx, sy, ex, ey;

	edge_plot = &CvMat(edge);
	lines_out_plot = &lines_out;
	min_len_plot = min_len;
	max_gap_plot = max_gap;

	CV_Assert(CV_IS_MASK_ARR(edge_plot));
	CV_Assert(CV_IS_MAT(edge_plot));
	CV_Assert(CV_MAT_TYPE(edge_plot->type) == CV_8UC1);


	const double topy = 0.0;
	const double boty = (double)edge.rows - 1;
	const double lefx = 0.0;
	const double rigx = (double)edge.cols - 1;

	double topx, botx, lefy, rigy;

	//CV_Assert( CV_IS_MAT(img) && CV_MAT_TYPE(img->type) == CV_8UC1 );

	class local {
	public:
		static void plot(int x, int y) {

			const uchar* image = edge_plot->data.ptr;
			int step = edge_plot->step;

			if (image[y * step + x] != 0) {
				// valid pixel
				if (sx == -1) {
					// we are currently not completing a line so start a new one
					sx = x;
					sy = y;

					len = 1;
					gap = 0;
				} else {
					// extend line
					ex = x;
					ey = y;

					len += gap + 1;
					gap = 0;
				}

			} else {
				// invalid pixel
				++gap;

				if (gap > max_gap_plot) {
					// gap too big, finish line if we have one
					if (sx != -1 && ex != -1 && len >= min_len_plot) {
						lines_out_plot->push_back(LineSeg(sx, sy, ex, ey));
					}

					// reset for next line
					len = 0;
					gap = 0;
					sx = -1;
					ex = -1;
				}
			}
		}
	};



	// iterate over all lines
	for (int k = 0; k < lines_in.size(); ++k) {
		// init data
		len = 0;
		gap = 0;
		sx = -1;
		ex = -1;

		topx = -1.0;
		botx = -1.0;
		lefy = -1.0;
		rigy = -1.0;

		// transform line data to u + t * v representation
		float d = lines_in[k][0];
		float a = lines_in[k][1];

		double ux = cos((double)a);
		double uy = sin((double)a);

		double vx = -uy;
		double vy =  ux;

		ux *= d;
		uy *= d;

		// get points of intersection with the image edges
		if (vy != 0.0) {
			topx = (ux + (topy - uy) / vy * vx);
			botx = (ux + (boty - uy) / vy * vx);
		}
		if (vx != 0.0) {
			lefy = (uy + (lefx - ux) / vx * vy);
			rigy = (uy + (rigx - ux) / vx * vy);
		}

		vector<int> line;
		if (lefx <= topx && topx <= rigx) {
			line.push_back((int)topx);
			line.push_back((int)topy);
		}
		
		if (lefx <= botx && botx <= rigx) {
			line.push_back((int)botx);
			line.push_back((int)boty);
		}
		
		if (topy <= lefy && lefy <= boty) {
			line.push_back((int)lefx);
			line.push_back((int)lefy);
		}
		
		if (topy <= rigy && rigy <= boty) {
			line.push_back((int)rigx);
			line.push_back((int)rigy);
		}

		if (line.size() != 4) {
			cout << "bhm_hough error " << line.size() << " " << k << endl;
			//exit(1);
		}

		// traverse points from one to the other edge
		bhm_plot(local::plot, line[0], line[1], line[2], line[3]);
	}

}


// counts the number of different pixels in lines1 and lines2
// c1 is the number of pixels which are unique to lines1
// c2 is the number of pixels which are unique to lines2
void bhm_difference(PixelStats& stats, const vector<LineSeg>& lines1, const vector<LineSeg>& lines2) {
	int minx	=  1000000;
	int miny	=  1000000;
	int maxx	= -1000000;
	int maxy	= -1000000;

	// determine buffer size
	for (int k = 0; k < lines1.size(); ++k) {
		const LineSeg& l = lines1[k];
		int cminx =  min(l.sx, l.ex);
		int cmaxx =  max(l.sx, l.ex);
		int cminy =  min(l.sy, l.ey);
		int cmaxy =  max(l.sy, l.ey);

		if (cminx < minx) minx = cminx;
		if (cminy < miny) miny = cminy;
		if (cmaxx > maxx) maxx = cmaxx;
		if (cmaxy > maxy) maxy = cmaxy;
	}

	for (int k = 0; k < lines2.size(); ++k) {
		const LineSeg& l = lines2[k];
		int cminx =  min(l.sx, l.ex);
		int cmaxx =  max(l.sx, l.ex);
		int cminy =  min(l.sy, l.ey);
		int cmaxy =  max(l.sy, l.ey);

		if (cminx < minx) minx = cminx;
		if (cminy < miny) miny = cminy;
		if (cmaxx > maxx) maxx = cmaxx;
		if (cmaxy > maxy) maxy = cmaxy;
	}

	int w = maxx - minx + 1;
	int h = maxy - miny + 1;
	assert(w > 0 && h > 0);

	stats.minx = minx;
	stats.miny = miny;

	static PixelStats& _stats = stats;
	static char* buffer;
	static int _c1, _c2, _w, _h;

	_stats = stats;
	buffer = new char[w*h];
	memset(buffer, 0, w*h);
	_c1 = 0;
	_c2 = 0;
	_w = w;
	_h = h;

	class local {
	public:
		static void plot1(int x, int y) {
			// first count all pixels of lines1 (once)
			if (buffer[x + y * _w] == 0) {
				++_stats.unique1;
				++_stats.total1;
				buffer[x + y * _w] = 1;
			} else {
				assert(buffer[x + y * _w] == 1);
				++_stats.overlap1;
			}
		}

		static void plot2(int x, int y) {
			if (buffer[x + y * _w] == 1) {
				// decrease pixel count (once) since pixel is apparently not unique
				--_stats.unique1;
				++_stats.total2;
				buffer[x + y * _w] = 2;
			} else if (buffer[x + y * _w] == 0) {
				// each new pixel at that stage is unique
				++_stats.unique2;
				++_stats.total2;
				buffer[x + y * _w] = 2;
				_stats.unique2_vec.push_back(cv::Vec2i(x, y));
			} else {
				assert(buffer[x + y * _w] == 2);
				++_stats.overlap2;
				_stats.overlap2_vec.push_back(cv::Vec2i(x, y));
			}
		}
	};

	// plot lines
	for (int k = 0; k < lines1.size(); ++k) {
		const LineSeg& l = lines1[k];
		bhm_plot(local::plot1, l.sx - minx, l.sy - miny, l.ex - minx, l.ey - miny);
	}

	for (int k = 0; k < lines2.size(); ++k) {
		const LineSeg& l = lines2[k];
		bhm_plot(local::plot2, l.sx - minx, l.sy - miny, l.ex - minx, l.ey - miny);
	}

	delete[] buffer;
}