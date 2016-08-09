#ifndef _QUADTREE_H_
#define _QUADTREE_H_

#include <vector>
#include <forward_list>

#include <opencv2\core\core.hpp>

using cv::Point;
using cv::Rect;
using std::forward_list;
using std::vector;

#define NullQNode ((QuadNode<value_t>*)0)


namespace pcv {
	
	enum Quadrant {
		NE, NW, SW, SE
	};

	template<typename value_t> class QuadNode;
	template<typename value_t> class QuadTree;
	
	template<typename value_t>
	class QuadNode {
	public:
		typedef QuadNode<value_t> QN;

		QN* parent;

		QN* ne;
		QN* nw;
		QN* sw;
		QN* se;

		Quadrant q;

		Rect bounds;

		forward_list<value_t> val;


		QuadNode(QN* parent, Quadrant q, Rect&& bounds) {
			this->parent = parent;
			this->ne = NullQNode;
			this->nw = NullQNode;
			this->sw = NullQNode;
			this->se = NullQNode;

			this->q = q;

			this->bounds = bounds;
		}

		QuadNode() {
			this->parent = NullQNode;
			this->ne = NullQNode;
			this->nw = NullQNode;
			this->sw = NullQNode;
			this->se = NullQNode;
		}

		inline bool isRoot() {
			return (parent == NullQNode);
		}

		inline bool isLeaf() {
			return (ne == NullQNode) && (nw == NullQNode) && (sw == NullQNode) && (se == NullQNode);
		}


		~QuadNode() {

		}
		
	};

	template<typename value_t>
	class QuadTree {
	public:
		typedef QuadNode<value_t> QN;
		
		QuadTree(int imgw, int imgh, int max_leaf_size);
		~QuadTree();


		QN* find(const Point& p);
		void find(const Point& p, float r, vector<QN*> nodes);
		QN* root();
		size_t size();

	private:
		int imgw, imgh;
		int max_leaf_size;
		size_t size_;
		QN* nodes_;
		QN* root_;

		QN* makeKidzz(QN* parent);
	};

}

void test_quadtree();


#endif