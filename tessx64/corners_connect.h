#ifndef _CORNERS_CONNECT_H_
#define _CORNERS_CONNECT_H_


#include "common.h"

#include "raster.h"
#include "graph.h"
#include "lineseg.h"


typedef pcv::Graph<Point, float>		Frame;
typedef Frame::Node						Corner;		// stores Point (corner position)
typedef Frame::Edge						Wall;		// stores float (wall thickness/width)
typedef Frame::HalfEdgeVal				HalfWall;
typedef Frame::ItNode					ItCorner;	// Iterator to Corner
typedef Frame::ItEdge					ItWall;		// Iterator to Wall

typedef pcv::Raster<HalfWall>			Raster_HW;
typedef Raster_HW::RP					RasterP_HW;
typedef Raster_HW::ItRP					ItRasterP_HW;




class PointPtrW {
public:
	Point* point;
	float width;

	PointPtrW() {
		point = (Point*)0;
		width = 0.0f;
	}

	PointPtrW(Point* p, float w) {
		point = p;
		width = w;
	}
};

class LineSegIter {
public:
	list<Corner>::iterator start;
	list<Corner>::iterator end;
	float width;

	LineSegIter(const list<Corner>::iterator& start, const list<Corner>::iterator& end, float width) {
		this->start = start;
		this->end = end;
		this->width = width;
	}
};


void corners_connect(vector<LineSegW>& linesw, Frame& frame);
void corners_connect2(vector<LineSegW>& linesw, Frame& frame);
void test_corners_connect();


#endif