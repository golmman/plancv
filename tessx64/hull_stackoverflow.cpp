
const char hull4_table[] = {		
	1,2,3,0,1,2,3,0,1,2,4,3,1,2,3,0,1,2,3,0,1,2,4,0,1,2,3,4,1,2,4,0,1,2,4,0,
	1,2,3,0,1,2,3,0,1,4,3,0,1,2,3,0,0,0,0,0,0,0,0,0,2,3,4,0,0,0,0,0,0,0,0,0,
	1,4,2,3,1,4,3,0,1,4,3,0,2,3,4,0,0,0,0,0,0,0,0,0,2,3,4,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,2,4,3,0,0,0,0,0,0,0,0,0,1,2,4,0,1,3,4,0,1,2,4,0,1,2,4,0,
	0,0,0,0,0,0,0,0,1,4,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,4,0,0,0,0,0,0,0,0,0,
	1,4,2,0,1,4,2,0,1,4,3,0,1,4,2,0,0,0,0,0,0,0,0,0,2,3,4,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,2,4,3,0,0,0,0,0,0,0,0,0,2,4,3,0,1,3,4,0,1,3,4,0,1,3,2,4,
	0,0,0,0,0,0,0,0,2,4,3,0,0,0,0,0,0,0,0,0,1,3,2,0,1,3,4,0,1,3,2,0,1,3,2,0,
	1,4,2,0,1,4,2,0,1,4,3,2,1,4,2,0,1,3,2,0,1,3,2,0,1,3,4,2,1,3,2,0,1,3,2,0
};

struct Vec2i {
	int x, y;
};

typedef long long int64;	

inline int sign(int64 x) {
	return (x > 0) - (x < 0);
}

inline int64 orientation(const Vec2i& a, const Vec2i& b, const Vec2i& c) {
	return (int64)(b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
}

void convex_hull4(const Vec2i** points) {
	const Vec2i* p[5] = {(Vec2i*)0, points[0], points[1], points[2], points[3]};
	char abc = (char)1 - sign(orientation(*points[0], *points[1], *points[2]));
	char abd = (char)1 - sign(orientation(*points[0], *points[1], *points[3]));
	char cad = (char)1 - sign(orientation(*points[2], *points[0], *points[3]));
	char bcd = (char)1 - sign(orientation(*points[1], *points[2], *points[3]));

	const char* t = hull4_table + (int)4 * (bcd + 3*cad + 9*abd + 27*abc);
	points[0] = p[t[0]];
	points[1] = p[t[1]];
	points[2] = p[t[2]];
	points[3] = p[t[3]];
}