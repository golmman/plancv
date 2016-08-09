#include "corners_connect.h"


bool compare_width(const LineSegW& l0, const LineSegW& l1) {
	return (l0.width > l1.width);	// descending
}

void cc_process(const Point& line_point, float width, pcv::Raster<PointPtrW>& raster) {
	// find points close to the line start
	vector<pcv::RasterPoint<PointPtrW>*> rpoints;
	raster.find(line_point, width, rpoints); // is width/2 better?

	// move all points to the line start, erase processed points from raster
	for (int j = 0; j < rpoints.size(); ++j) {
		rpoints[j]->value.point->x = line_point.x;
		rpoints[j]->value.point->y = line_point.y;
		raster.erase(rpoints[j]);
	}
}

void corners_connect(vector<LineSegW>& linesw, Frame& frame) {
	
	// insert Corners into Frame
	vector<LineSegIter> lineseg_iters;
	for (int k = 0; k < linesw.size(); ++k) {
		auto start = frame.push_node(linesw[k].s);
		auto end = frame.push_node(linesw[k].e);
		lineseg_iters.push_back(LineSegIter(start, end, linesw[k].width)); // remember the connectivity
	}

	// build Walls
	for (int k = 0; k < lineseg_iters.size(); ++k) {
		LineSegIter& lsi = lineseg_iters[k];
		//frame.push_edge(lsi.width, *lsi.start, *lsi.end);
	}

	// make large widths appear first
	std::sort(linesw.begin(), linesw.end(), compare_width);

	// determine relevant image size
	int maxx = -INT_MAX;
	int maxy = -INT_MAX;
	for (int k = 0; k < linesw.size(); ++k) {
		int x = max(linesw[k].sx, linesw[k].ex);
		int y = max(linesw[k].sy, linesw[k].ey);
		if (x > maxx) maxx = x;
		if (y > maxy) maxy = x;
	}

	// put lines into raster with maximal 10k RasterPoints
	// TODO: what is a good grid_size value?	(1kk points seems to be too much)
	int grid_size = max(maxx+1, maxy+1) / 100 + 1;
	pcv::Raster<PointPtrW> raster(maxx+1, maxy+1, grid_size);
	for (int k = 0; k < linesw.size(); ++k) {
		pcv::RasterPoint<PointPtrW> raster_point_s(PointPtrW(&linesw[k].s, linesw[k].width), linesw[k].s);
		pcv::RasterPoint<PointPtrW> raster_point_e(PointPtrW(&linesw[k].e, linesw[k].width), linesw[k].e);
		raster.push(raster_point_s);
		raster.push(raster_point_e);
	}

	//TODO: put doubling to extra method, add Frame support
	// process lines, start with thick ones
	for (int k = 0; k < linesw.size(); ++k) {
		cc_process(linesw[k].s, linesw[k].width, raster);
		cc_process(linesw[k].e, linesw[k].width, raster);
	}

}









void cc_process2(const Point& corner_point, float width, pcv::Raster<HalfWall>& raster, Frame& frame) {
	// find points close to the line start
	list<ItRasterP_HW> rpoints;
	raster.find(corner_point, width, rpoints); // is width/2 better?

	// collect corners, erase processed points from raster
	list<ItCorner> corners;
	for (auto rasp = rpoints.begin(); rasp != rpoints.end(); ++rasp) {
		corners.push_front((*rasp)->value.node);
		raster.erase(*rasp);
	}

	// merge the corners
	frame.merge_nodes(corners, corner_point);
}


void corners_connect2(vector<LineSegW>& linesw, Frame& frame) {
	
	// make large widths appear first
	std::sort(linesw.begin(), linesw.end(), compare_width);

	// build Frame
	for (int k = 0; k < linesw.size(); ++k) {
		ItCorner start = frame.push_node(linesw[k].s);
		ItCorner end = frame.push_node(linesw[k].e);
		frame.push_edge(linesw[k].width, start, end);
	}

	// determine relevant image size
	int maxx = -INT_MAX;
	int maxy = -INT_MAX;
	for (int k = 0; k < linesw.size(); ++k) {
		int x = max(linesw[k].sx, linesw[k].ex);
		int y = max(linesw[k].sy, linesw[k].ey);
		if (x > maxx) maxx = x;
		if (y > maxy) maxy = x;
	}

	// put lines into raster with maximal 10k RasterPoints
	// TODO: what is a good grid_size value?	(1kk points seems to be too much)
	int grid_size = max(maxx+1, maxy+1) / 100 + 1;
	Raster_HW raster(maxx+1, maxy+1, grid_size);
	for (ItWall wall = frame.edge_list().begin(); wall != frame.edge_list().end(); ++wall) {
		RasterP_HW raster_point_s(HalfWall(wall->val, wall->node0), wall->node0->val);
		RasterP_HW raster_point_e(HalfWall(wall->val, wall->node1), wall->node1->val);
		raster.push(raster_point_s);
		raster.push(raster_point_e);
	}

	// process walls, copy frame.edge_list since it may change
	list<Wall> wall_list(frame.edge_list());
	for (ItWall wall = wall_list.begin(); wall != wall_list.end(); ++wall) {
		cc_process2(wall->node0->val, wall->val, raster, frame);
		cc_process2(wall->node1->val, wall->val, raster, frame);
	}

}



void test_corners_connect() {
	
	vector<LineSegW> v;
	Frame frame;

	v.push_back(LineSegW(rand()%100, 0, 0, 0, 1.0));

	corners_connect2(v, frame);


	list<int> vv;
	vv.push_front(10);
	vv.push_front(1);
	vv.push_front(30);
	vv.push_front(9);


	auto val = vv.begin();

	while (val != vv.end()) {
	//for (auto val = vv.begin(); val != vv.end(); ++val) {
		if (*val == 30) {
			auto val_erase = val;
			++val;
			vv.erase(val_erase);
			continue;
		}

		++val;
	}
	cout << "  " << endl;
	for (auto val = vv.begin(); val != vv.end(); ++val) {
		 cout << *val << endl;
	}
}