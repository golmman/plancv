/*
	Simple container class for 2D points. 
	The goal is, given one point p, find all points near it (implemented as l2 nearness).
	So the idea is to downsample a picture and store all the pixel data in a grid,
	near grids are fastly found and only the points in those grids have to be compared to p.


*/ 



#ifndef _RASTER_H_
#define _RASTER_H_

#include "common.h"

#include <vector>
#include <list>
#include <opencv2\core\core.hpp>

using cv::Point;

using std::vector;
using std::list;


namespace pcv {
	
	template<typename value_t>
	class RasterPoint : public Point {
	public:
		value_t value;

		RasterPoint(const value_t& value, const Point& point) {
			this->x = point.x;
			this->y = point.y;
			this->value = value;
		}
	};


	template<typename value_t>
	class Raster {
	public:
		typedef RasterPoint<value_t>			RP;
		typedef typename list<RP>::iterator		ItRP;

		list<RP>* lists;

		~Raster() {
			delete[] lists;
		}

		Raster(int imgw, int imgh, int grid_size) {
			imgw_ = imgw;
			imgh_ = imgh;
			grid_size_ = grid_size;
			grid_w_ = imgw / grid_size + 1;
			grid_h_ = imgh / grid_size + 1;
			size_ = grid_w_ * grid_h_;
			
			lists = new list<RP>[size_];
		}

		// Returns the number of lists, i.e. the number of grids.
		size_t size() {
			return size_;
		}

		int grid_size() {
			return grid_size_;
		}

		// DEPRECATED
		// Push RasterPoint to this Raster.
		RP* push2(const RP& rp) {
			assert(0 <= rp.x && rp.x < imgw_ && 0 <= rp.y && rp.y < imgh_);
			int x = rp.x / grid_size_;
			int y = rp.y / grid_size_;
			lists[x + y * grid_w_].push_front(rp);
			return &lists[x + y * grid_w_].front();
		}

		// DEPRECATED
		// Erase a RasterPoint from this Raster.
		void erase(RP* rp) {
			assert(0 <= rp->x && rp->x < imgw_ && 0 <= rp->y && rp->y < imgh_);
			int x = rp->x / grid_size_;
			int y = rp->y / grid_size_;
			list<RP>& l = lists[x + y * grid_w_];

			for (auto val = l.begin(); val != l.end(); ++val) {
				if (&(*val) == rp) {
					l.erase(val);
					break;
				}
			}
		}

		// DEPRECATED
		// Find all values which are in the circle of radius r around p and push them to the given vector.
		void find(const Point& p, double r, vector<RP*>& values) {
			// convert input circle to grid coordinates

			Point grid_p(p.x / grid_size_, p.y / grid_size_);
			int grid_r = (int)(r / grid_size_) + 1; // TODO: +1 is lazy, this can be optimised

			int grid_minx = max(	  0, grid_p.x - grid_r);
			int grid_miny = max(	  0, grid_p.y - grid_r);
			int grid_maxx = min(grid_w_, grid_p.x + grid_r);
			int grid_maxy = min(grid_h_, grid_p.y + grid_r);

			// find all grids in l1 distance
			for (int y = grid_miny; y < grid_maxy; ++y) {
				for (int x = grid_minx; x < grid_maxx; ++x) {
			
					// copy all values from the grid to the vector if distance is ok
					list<RP>& l = lists[x + y * grid_w_];
					for (auto val = l.begin(); val != l.end(); ++val) {
						if (fast_dist(*val, p) <= r) {
							values.push_back(&(*val));
						}
					}

				}
			}
		}




		// Push RasterPoint to this Raster.
		ItRP push(const RP& rp) {
			assert(0 <= rp.x && rp.x < imgw_ && 0 <= rp.y && rp.y < imgh_);
			int x = rp.x / grid_size_;
			int y = rp.y / grid_size_;
			lists[x + y * grid_w_].push_front(rp);
			return lists[x + y * grid_w_].begin();
		}

		// Erase a RasterPoint from this Raster.
		void erase(ItRP rp) {
			assert(0 <= rp->x && rp->x < imgw_ && 0 <= rp->y && rp->y < imgh_);
			int x = rp->x / grid_size_;
			int y = rp->y / grid_size_;
			list<RP>& l = lists[x + y * grid_w_];

			l.erase(rp);
		}

		// Find all values which are in the circle of radius r around p and push them to the given vector.
		void find(const Point& p, double r, list<ItRP>& values) {
			// convert input circle to grid coordinates

			Point grid_p(p.x / grid_size_, p.y / grid_size_);
			int grid_r = (int)(r / grid_size_) + 1; // TODO: +1 is lazy, this can be optimised

			int grid_minx = max(	  0, grid_p.x - grid_r);
			int grid_miny = max(	  0, grid_p.y - grid_r);
			int grid_maxx = min(grid_w_, grid_p.x + grid_r);
			int grid_maxy = min(grid_h_, grid_p.y + grid_r);

			// find all grids in l1 distance
			for (int y = grid_miny; y < grid_maxy; ++y) {
				for (int x = grid_minx; x < grid_maxx; ++x) {
			
					// copy all values from the grid to the vector if distance is ok
					list<RP>& l = lists[x + y * grid_w_];
					for (auto val = l.begin(); val != l.end(); ++val) {
						if (fast_dist(*val, p) <= r) {
							values.push_back(val);
						}
					}

				}
			}
		}



		
	private:
		int imgw_, imgh_;
		int grid_w_, grid_h_;
		int grid_size_;
		size_t size_;
	};

}




void test_raster();


#endif