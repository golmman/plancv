#include "quadtree.h"

#include <opencv2\core\core.hpp>
#include <set>
#include <stack>
#include <iostream>

using cv::Vec;
using std::set;
using std::stack;

using std::cout;
using std::endl;

/*

	NOOB ALERT - template methods only in headers...

*/


template<typename value_t>
pcv::QuadTree<value_t>::~QuadTree() {
	free(root_);
}

template<typename value_t>
pcv::QuadTree<value_t>::QuadTree(int imgw, int imgh, int max_leaf_size) {
	this->imgw = imgw;
	this->imgh = imgh;
	this->max_leaf_size = max_leaf_size;

	root_ = (QN*)malloc((size_t)(1.34 * 4 * (imgw/max_leaf_size+1) * (imgh/max_leaf_size+1) * sizeof(QN)));
	nodes_ = root_;

	(*nodes_) = QN(NullQNode, NE, Rect(0, 0, imgw, imgh));

	makeKidzz(nodes_);

	size_ = (size_t)(nodes_ - root_);
}

template<typename value_t>
pcv::QuadNode<value_t>* pcv::QuadTree<value_t>::makeKidzz(QN* parent) {
	int x = parent->bounds.x;
	int y = parent->bounds.y;
	int w = parent->bounds.width;
	int h = parent->bounds.height;

	if (w == 0 || h == 0) {
		return NullQNode;
	}

	++nodes_;

	if (w < max_leaf_size || h < max_leaf_size) {
		parent->ne = NullQNode;
		parent->nw = NullQNode;
		parent->sw = NullQNode;
		parent->se = NullQNode;

		if (x+w > imgw) {
			parent->bounds.width = imgw - x;
		}
		if (y+h > imgh) {
			parent->bounds.height = imgh - y;
		}

		return parent;
	}
			
	(*nodes_) = QN(parent, NE, Rect(x + w/2, y + h/2, w/2 + 1, h/2 + 1));
	parent->ne = makeKidzz(nodes_);

	(*nodes_) = QN(parent, NW, Rect(x, y + h/2, w/2, h/2 + 1));
	parent->nw = makeKidzz(nodes_);

	(*nodes_) = QN(parent, SW, Rect(x, y, w/2, h/2));
	parent->sw = makeKidzz(nodes_);

	(*nodes_) = QN(parent, SE, Rect(x + w/2, y, w/2 + 1, h/2));
	parent->se = makeKidzz(nodes_);

	return parent;
}

template<typename value_t>
pcv::QuadNode<value_t>* pcv::QuadTree<value_t>::root() {
	return root_;
}

template<typename value_t>
size_t pcv::QuadTree<value_t>::size() {
	return size_;
}

template<typename value_t>
pcv::QuadNode<value_t>* pcv::QuadTree<value_t>::find(const Point& p) {
	assert(0 <= p.x && p.x < imgw && 0 <= p.y && p.y < imgh);

	QN* node = root_;

	while (!node->isLeaf()) {
		if (node->ne->bounds.contains(p)) {
			node = node->ne;
		} else if (node->nw->bounds.contains(p)) {
			node = node->nw;
		} else if (node->sw->bounds.contains(p)) {
			node = node->sw;
		} else if (node->se->bounds.contains(p)) {
			node = node->se;
		} else {
			assert(0);
		}
	}

	return node;
}

template<typename value_t>
void pcv::QuadTree<value_t>::find(const Point& p, float r, vector<QN*> nodes) {
	assert(0 <= p.x && p.x < imgw && 0 <= p.y && p.y < imgh);

	QN* node = root_;

	return node;
}


void test_quadtree() {
	
	//pcv::QuadNode<int*>* qns = (pcv::QuadNode<int*>*)malloc(87000 * sizeof(pcv::QuadNode<int*>));

	//for (int k = 0; k < 87000; ++k) {
	//	qns[k] = pcv::QuadNode<int*>((pcv::QuadNode<int*>*)0, pcv::NE, Rect(rand()%100, rand()%100, rand()%100, rand()%100));
	//}
	//cout << "clear..." << endl;
	//system("pause");
	//free(qns);




	//Point* arr = new Point[10];
	//Point* i = arr;

	//for (int k = 0; k < 10; ++k) {
	//	(*i++) = Point(k, k*k);
	//}

	//for (int k = 0; k < 10; ++k) {
	//	cout << arr[k] << endl;
	//}

	//cout << i - arr << endl;

	//delete[] arr;




	pcv::QuadTree<int*> qt(7000, 7000, 50);

	cout << qt.size() << endl;
	cout << qt.find(Point(0, 0))->bounds << endl;

	//int n = 0;

	//for (auto node = qt.nodes.begin(); node != qt.nodes.end(); ++node) {
	//	++n;
	//}

	//cout << n << " * " << sizeof(pcv::QuadNode<int*>) << endl;

	//qt.nodes.clear();

	//cout << "clear" << endl;

}
