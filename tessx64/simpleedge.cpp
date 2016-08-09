#include "simpleedge.h"

void edges_detect(const Mat& src, Mat& dst) {
	
	int h = src.rows;
	int w = src.cols;

	dst = image_create(w, h, 8);
	
	// find relevant pixels, ignore image edges
	for (int y = 1; y < h-1; ++y) {
		for (int x = 1; x < w-1; ++x) {
			if (pixel_read(src, x, y)) {
				uint8 n = 
					pixel_read(src, x-1, y-1) & pixel_read(src, x-0, y-1) & pixel_read(src, x+1, y-1) & 
					pixel_read(src, x-1, y-0) &							    pixel_read(src, x+1, y-0) & 
					pixel_read(src, x-1, y+1) & pixel_read(src, x-0, y+1) & pixel_read(src, x+1, y+1);

				if (n == 0) {
					// at least one neighbour is zero, i.e. (x, y) is an edge point
					pixel_draw(dst, x, y, pixel_read(src, x, y));
				}
			}
		}
	}
}