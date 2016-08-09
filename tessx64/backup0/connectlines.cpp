#include "connectlines.h"
#include "linefit.h"



double global_max_dist;
int global_max_gap;



// returns proper angle of input vector (range 0 to 2pi)
double angle2d(const cv::Vec2d& v) {
	cv::Vec2d x, y;

	x[0] = fast_norm(v) - v[0];
	x[1] = -v[1];

	y[0] = fast_norm(v) + v[0];
	y[1] = v[1];

	double ac = 2.0 * atan2(fast_norm(x), fast_norm(y));

	if (v[1] < 0.0) {
		ac = 2.0 * CV_PI - ac;
	}

	return ac;
}

// returns a line angle
// lines with angles which have difference pi are the same, so range is 0 to pi
// i.e. proper angle [0, pi] maps to [0, pi] and [pi, 2pi] maps to [0, pi]
double angle_line(const cv::Vec2d& v) {
	cv::Vec2d x, y;

	x[0] = fast_norm(v) - v[0];
	x[1] = -v[1];

	y[0] = fast_norm(v) + v[0];
	y[1] = v[1];

	double ac = 2.0 * atan2(fast_norm(x), fast_norm(y));

	if (v[1] < 0.0) {
		ac = CV_PI - ac;
	}

	return ac;
}

// compares two line angles, returns true if |a1 - a2| <= margin,
// pi is identified as 0 so this discontinuity is treated here
// USE ONLY TO COMPARE LINE ANGLES (as returned by angle_line)
bool angle_equiv(double a1, double a2, double m) {

	if (abs(a1 - a2) <= m) {
		// the trivial case
		return true;

	} else if (a1 <= m) {
		// but there are more cases for which we want to return true
		a1 += CV_PI;
		if (a1 - a2 <= m) {
			return true;
		}

	} else if (a2 <= m) {
		a2 += CV_PI;
		if (a2 - a1 <= m) {
			return true;
		}

	}

	return false;
}

bool dist_equiv(double d1, double d2, double m) {
	if (abs(d1 - d2) <= m) {
		return true;
	}
	return false;
}


void getAandD(double& angle, double& distance, const Vec4i& line) {
	Vec2d v(line[2] - line[0], line[3] - line[1]);	// direction vector of the line
	Vec2d p(line[0], line[1]);						// support vector of line, i.e. f(t) = p + t * v
	angle = angle_line(v);							// angle of line

	double t = -v.ddot(p) / v.ddot(v);				// calculate perpendicular
	p[0] += t * v[0];
	p[1] += t * v[1];
	distance = fast_norm(p);							// distance of line
}

// distance between line segment and point
// using that line segments from s to e can be described by the set
// {x | |x-e| + |x-s| = |e-s|}
// the term ||x-e| + |x-s| - |e-s|| has 2*|e-s| asymptotics hence the  divide
double dist_lp_var(const LineSeg& l, int x, int y) {
	double d = fast_dist(l.e, l.s);
	return abs(fast_norm(x-l.ex, y-l.ey) + fast_norm(x-l.sx, y-l.sy) - d) / (2.0 * d);
}

// distance between two line segments using dist_lp_var
double dist_ll_var(const LineSeg& l0, const LineSeg& l1) {
	double d0, d1, d2, d3;

	d0 = dist_lp_var(l0, l1.sx, l1.sy);
	d1 = dist_lp_var(l0, l1.ex, l1.ey);
	d2 = dist_lp_var(l1, l0.sx, l0.sy);
	d3 = dist_lp_var(l1, l0.sx, l0.sy);

	return std::max(std::max(d0, d1), std::max(d2, d3));
}

// https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
// Returns the maximal distance of the start and endpoints to the other line.
// The returned value is an upper bound for the width of a minimum area enclosing rectangle.
template <typename line_t>
double dist_ll(const line_t& l0, const line_t& l1) {
	double d0, d1, d2, d3, n;
	int64 a, dx, dy;

	a = l0.ex * l0.sy - l0.ey * l0.sx;
	dx = l0.ex - l0.sx;
	dy = l0.ey - l0.sy;
	n = sqrt(dx * dx + dy * dy);
	d0 = (double)abs(dy * l1.sx - dx * l1.sy + a) / n;	// dist(l0, l1.s)
	d1 = (double)abs(dy * l1.ex - dx * l1.ey + a) / n;	// dist(l0, l1.e)

	a = l1.ex * l1.sy - l1.ey * l1.sx;
	dx = l1.ex - l1.sx;
	dy = l1.ey - l1.sy;
	n = sqrt(dx * dx + dy * dy);
	d2 = (double)abs(dy * l0.sx - dx * l0.sy + a) / n;	// dist(l1, l0.s)
	d3 = (double)abs(dy * l0.ex - dx * l0.ey + a) / n;	// dist(l1, l0.e)

	return max(max(d0, d1), max(d2, d3));
}


inline bool early_reject(const LineSeg& l0, const LineSeg& l1) {
	return (dist_ll(l0, l1) > global_max_dist);
}

inline bool early_reject(const LineSegW& l0, const LineSegW& l1) {
	//return (dist_ll(l0, l1) > 0.5 * max(l0.width, l1.width));
	//return (dist_ll(l0, l1) > 0.5 * (l0.width + l1.width));
	return (dist_ll(l0, l1) > max(l0.width, l1.width));
}


// Decides if two lines should be put in the same connected component 
// by first testing if they are in the same plane with dist_ll,  
// then if they are actually close by projecting their points to the middle line between them.
template <typename line_t>	// "line_t extends Line" not existent in C++, sucks
bool connectable(const line_t& l0, const line_t& l1) {
	//LineSeg& l0 = (LineSeg&)line0;
	//LineSeg& l1 = (LineSeg&)line1;

	// test if lines are in the same plane
	if (early_reject(l0, l1)) {
		return false;
	}

	// Lines lie approximately in the same plane.
	// Determine the line in the middle of both lines,
	// then project start- and endpoints to that new line and compare their positions.

	Vec2f v1, v2, v3;
	double t, t0s, t0e, t1s, t1e;

	v1 = Vec2f((float)(l0.ex - l0.sx), (float)(l0.ey - l0.sy));
	v1 /= fast_norm(v1);
	v2 = Vec2f((float)(l1.ex - l1.sx), (float)(l1.ey - l1.sy));
	v2 /= fast_norm(v2);

	// v3 should be in the acute angle
	if (v1.ddot(v2) < 0) {
		v2 = -v2;
	}

	assert(v1 + v2 != Vec2f(0.0, 0.0));		// prevent division by 0
	assert(v1.ddot(v2) > 0);

	v3 = v1 + v2;				// calculate middle line: t maps to t*v3
	v3 /= fast_norm(v3);
		
	t0s = v3.ddot(l0.s);		// project start- and endpoints to the new line
	t0e = v3.ddot(l0.e);
	t1s = v3.ddot(l1.s);
	t1e = v3.ddot(l1.e);

	// make sure end points have higher t value, swap if necessary
	if (t0s > t0e) {
		t = t0s; t0s = t0e;	t0e = t;
	}
	if (t1s > t1e) {
		t = t1s; t1s = t1e;	t1e = t;
	}

	// compare positions
	if (t1s > t0e) {
		// no intesection, l1 lies completly behind l0
		if (t1s - t0e <= global_max_gap) {
			return true;
		} else {
			return false;
		}
	} else if (t1e < t0s) {
		// no intesection, l1 lies completly in front of l0
		if (t0s - t1e <= global_max_gap) {
			return true;
		} else {
			return false;
		}
	} 

	// projection intervals intersect
	return true;
}

// Merge a connected compontent of lines to a new line with width.
void merge(const forward_list<LineSeg*>& component, LineSegW& out) {
	// TODO: in principle no loop/collection is necessary
	// Note: profiling indicates this is not even close to a bottleneck, so ok for now
	forward_list<Vec2i*> component2i;
	for (auto vec = component.begin(); vec != component.end(); ++vec) {
		component2i.push_front(&(*vec)->s);
		component2i.push_front(&(*vec)->e);
		//component2i.push_front((Vec2i*)&(*vec)->val[0]);
		//component2i.push_front((Vec2i*)&(*vec)->val[2]);
	}

	line_fit(component2i, out);
}


// Merge a connected compontent of lines to a new line with width.
void merge(const forward_list<LineSegW*>& component, LineSegW& out) {
	// TODO: in principle no loop/collection is necessary
	// Note: profiling indicates this is not even close to a bottleneck, so ok for now

	float maxw = 0.0f;

	forward_list<Vec2i*> component2i;
	for (auto vec = component.begin(); vec != component.end(); ++vec) {
		component2i.push_front(&(*vec)->s);
		component2i.push_front(&(*vec)->e);
		if ((*vec)->width > maxw) {
			maxw = (*vec)->width;
		}
	}

	line_fit(component2i, out);
	out.width = max(maxw, out.width);
}



// Finds connected components in input set using the function connectable  
// then merges those compontnes to an output type as specified by the function merge.
// Note on complexity: 
// Let n be the number of input elements and c (<= n) the number of connected components,
// then we have typically O(n^2) connectable calls 
// but only c merge calls (processing n elements in total). So connectable should be a fast function.
// One can reduce the number of connectable calls by ordering the input set 
// such that connectable returns true for early elements often.
template <class input_t, class output_t>
void rewrite_set(
	vector<input_t>& setin, 
	vector<output_t>& setout, 
	bool (*connectable)(const input_t&, const input_t&), 
	void (*merge)(const forward_list<input_t*>&, output_t&)) 
{
	int dist_ll_count = 0;

	forward_list<forward_list<input_t*>> components;
	
	// create connected components
	for (int i = 0; i < setin.size(); ++i) {
		input_t* elem0 = &setin[i];
		bool inserted = false;

		// try to find a connected component where elem0 belongs to
		for (auto comp = components.begin(); comp != components.end(); ++comp) {
			for (auto elem1 = comp->begin(); elem1 != comp->end(); ++elem1) {
				++dist_ll_count;
				if (connectable(*elem0, *(*elem1))) {
					// we found an already exisiting component where elem0 fits in
					comp->push_front(elem0);
					inserted = true;
					break;
				}
			}
			if (inserted) break;
		}

		if (!inserted) {
			// no component fits elem0, create a new one
			components.push_front(forward_list<input_t*>());
			components.front().push_front(elem0);

			//forward_list<input_t*> comp;
			//comp.push_front(elem0);
			//components.push_front(comp);
		}
	}

	// merge the components and put them into a new vector
	output_t newelem;
	for (auto comp = components.begin(); comp != components.end(); ++comp) {
		merge(*comp, newelem);
		setout.push_back(newelem);
	}

	PCV_DEBUG(cout << "dist_ll_count: " << dist_ll_count << " ");
}

// Connects lines by using rewrite_set, see documentation of used functions.
void linesw_connect(vector<LineSeg>& lines, vector<LineSegW>& linesw,  int max_gap, double max_dist) {
	global_max_gap = max_gap;
	global_max_dist = max_dist;

	rewrite_set(lines, linesw, connectable<LineSeg>, merge);
}

void linesw_connect(vector<LineSegW>& lines, vector<LineSegW>& linesw,  int max_gap) {
	global_max_gap = max_gap;

	rewrite_set(lines, linesw, connectable<LineSegW>, merge);
}


// Connects lines in place
void lines_connect(vector<LineSeg>& lines, int max_gap, double max_dist) {

	int dist_ll_count = 0;

	Vec2f v1, v2, v3;
	double t, t0s, t0e, t1s, t1e;

	int tmp;
	int c1=0, c2=0, c3=0, c4=0, c5=0, c6=0;

	int changes = -1;
	int iteration_count = 0;

	cout << lines.size() << "->";


	while (changes != 0) {
		if ((iteration_count+1) % 50 == 0) {
			cout << iteration_count << " connect_line iterations, big file or error? changes last iteration: " << changes << endl;
		}

		++iteration_count;
		changes = 0;

		for(int i0 = 0; i0 < lines.size(); ++i0) {
			LineSeg& l0 = lines[i0];
		
			for (int i1 = i0 + 1; i1 < lines.size(); ++i1) {
				LineSeg& l1 = lines[i1];
				++dist_ll_count;
				if (dist_ll(l0, l1) <= max_dist) {
					// lines lie approximately in the same plane
					// determine the line in the middle of both lines
					// then project start- and endpoints to that new line and compare their positions

					v1 = Vec2f((float)(l0.ex - l0.sx), (float)(l0.ey - l0.sy));
					v1 /= fast_norm(v1);
					v2 = Vec2f((float)(l1.ex - l1.sx), (float)(l1.ey - l1.sy));
					v2 /= fast_norm(v2);

					// v3 should be in the acute angle
					if (v1.ddot(v2) < 0) {
						v2 = -v2;
					}

					assert(v1 + v2 != Vec2f(0.0, 0.0));		// prevent division by 0
					assert(v1.ddot(v2) > 0);

					v3 = v1 + v2;				// calculate middle line: t maps to t*v3
					v3 /= fast_norm(v3);
		
					t0s = v3.ddot(l0.s);		// project start- and endpoints to the new line
					t0e = v3.ddot(l0.e);
					t1s = v3.ddot(l1.s);
					t1e = v3.ddot(l1.e);

					// make sure end points have higher t value
					if (t0s > t0e) {	// for l0
						t = t0s; t0s = t0e;	t0e = t;
						// swap start and end point
						tmp = l0.sx; l0.sx = l0.ex;	l0.ex = tmp;	// swap x
						tmp = l0.sy; l0.sy = l0.ey;	l0.ey = tmp;	// swap y
					}

					if (t1s > t1e) {	// for l1
						t = t1s; t1s = t1e;	t1e = t;
						// swap start and end point
						tmp = l1.sx; l1.sx = l1.ex;	l1.ex = tmp;	// swap x
						tmp = l1.sy; l1.sy = l1.ey;	l1.ey = tmp;	// swap y
					}

					// compare positions
					if (t1s >= t0s) {
						if (t1s <= t0e) {

							if (t1e <= t0e) {

								// l1 lies inside l0
								fast_erase(lines, i1);
								++c1; ++changes;
								break;

							} else {

								// l0 and l1 intersect, where l0 starts earlier and l1 goes further
								l1.s = l0.s;
								fast_erase(lines, i0);
								--i0;	// do not skip new current element
								++c2; ++changes;
								break;
							}

						} else {
							// no intesection, l1 lies completly behind l0
							if (t1s - t0e <= max_gap) {

								// distance between lines is smaller than gap, so connect them
								l1.s = l0.s;
								fast_erase(lines, i0);
								--i0;	// do not skip new current element
								++c3; ++changes;
								break;
							}
						}
					} else if (t1e >= t0s) {

						if (t1e <= t0e) {
							// l0 and l1 intersect, where l1 starts earlier and l0 goes further
							l1.e = l0.e;
							fast_erase(lines, i0);
							--i0;	// do not skip new current element
							++c4; ++changes;
							break;
						} else {
							// l0 lies inside l1
							fast_erase(lines, i0);
							--i0;	// do not skip new current element
							++c5; ++changes;
							break;
						}

					} else {
						// no intesection, l1 lies completly in front of l0
						if (t0s - t1e <= max_gap) {
							// distance between lines is smaller than gap, so connect them
							l1.e = l0.e;
							fast_erase(lines, i0);
							--i0;	// do not skip new current element
							++c6; ++changes;
							break;
						}
					}

				}
			}
		}

		cout << changes << endl;
	}

	PCV_DEBUG(cout << lines.size() << ", iterations: " << iteration_count << ", ");
	PCV_DEBUG(cout << "cases: |" << c1 << " " << c2 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << "| " << endl);
	PCV_DEBUG(cout << "dist_ll_count: " << dist_ll_count << " ");
}
