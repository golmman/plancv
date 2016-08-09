#ifndef _LINESEG_H_
#define _LINESEG_H_

#include "common.h"

class LineSeg;
class LineSegW;


class CornerValue {
public:

};

class WallValue {
public:

};


class LineSeg {
public:
	union {
		struct {
			int sx;
			int sy;
			int ex;
			int ey;
		};

		struct {
			Point s;
			Point e;
		};
	};
	
	// see LineSegW, c++ sucks
	//LineSeg(int sx, int sy, int ex, int ey) : 
	//	sx(sx), sy(sy), ex(ex), ey(ey) {}

	LineSeg() {}

	LineSeg(const LineSeg& l) {
		sx = l.sx;
		sy = l.sy;
		ex = l.ex;
		ey = l.ey;
	}

	LineSeg(int _sx, int _sy, int _ex, int _ey) {
		sx = _sx;
		sy = _sy;
		ex = _ex;
		ey = _ey;
	}

	inline int& operator[](int i) {
		return ((Vec4i*)this)->val[i];
		//return reinterpret_cast<int&>(*(reinterpret_cast<int*>(this) + i));
	}
};

class LineSegW {
public:
	union {
		struct {
			int sx;
			int sy;
			int ex;
			int ey;
		};

		struct {
			Point s;
			Point e;
		};

		struct {
			LineSeg lineseg;
		};
	};

	float width;
	float length;
		
	LineSegW() {}

	LineSegW(const LineSegW& l) {
		sx = l.sx;
		sy = l.sy;
		ex = l.ex;
		ey = l.ey;
		width = l.width;
		length = l.length;
	}

	/*
	 * member list initialisation does not work here
	 * apparently since unnamed struct are not standard (still no warning or error?!)
	 * c++ sucks!
	 */
	//LineSegW(int _sx, int _sy, int _ex, int _ey, int _w) : 
	//	sx(_sx), sy(_sy), ex(_ex), ey(_ey), w(_w) {}

	LineSegW(int _sx, int _sy, int _ex, int _ey, float _w) {
		sx = _sx;
		sy = _sy;
		ex = _ex;
		ey = _ey;
		width = _w;

		double dx = ex - sx;
		double dy = ey - sy;
		length = (float)sqrt(dx * dx + dy * dy);
	}


	inline LineSeg& getLineSeg() {
		return *((LineSeg*)this);
	}

	void draw(Mat& img, const Scalar& color) {
		float w2 = this->width / 2;
		Point poly[4];
		Point2f perp(-(float)(this->ey - this->sy), (float)(this->ex - this->sx));
		perp /= this->length;
		poly[0] = Point((int)(this->sx + w2 * perp.x), (int)(this->sy + w2 * perp.y));
		poly[1] = Point((int)(this->ex + w2 * perp.x), (int)(this->ey + w2 * perp.y));
		poly[2] = Point((int)(this->ex - w2 * perp.x), (int)(this->ey - w2 * perp.y));
		poly[3] = Point((int)(this->sx - w2 * perp.x), (int)(this->sy - w2 * perp.y));

		fillConvexPoly(img, poly, 4, color);
	}
	
};


static inline std::ostream& operator<<(std::ostream& out, const LineSeg& l) {
	return out << "[" << l.sx << ", " << l.sy << ", " << l.ex << ", " << l.ey << ", " << "]";
}

static inline std::ostream& operator<<(std::ostream& out, const LineSegW& l) {
	return out << "[" << l.sx << ", " << l.sy << ", " << l.ex << ", " << l.ey << ", " << l.width << ", " << l.length << "]";
}


#endif