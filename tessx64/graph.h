#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <list>
#include <vector>
#include <stack>
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include <unordered_set>

using std::list;
using std::vector;
using std::stack;
using std::unordered_map;
using std::unordered_set;
using std::cout;
using std::endl;

namespace pcv {

	template<typename node_t, typename edge_t> class Node_;
	template<typename node_t, typename edge_t> class Edge_;
	template<typename node_t, typename edge_t> class Graph;


	template<typename node_t, typename edge_t>
	class IteratorBreadth {
	public:
		
	};

	/*
	 *	class Node
	 */
	template<typename node_t, typename edge_t>
	class Node_ {
	template<typename node_t, typename edge_t> friend class Graph;
	public:
		typedef Edge_<node_t, edge_t>			Edge;
		typedef typename list<Edge>::iterator	ItEdge;

		node_t val;
		
		Node_(node_t&& val) {
			this->val = val;
		}

		Node_(const node_t& val) {
			this->val = val;
		}

		inline const list<ItEdge>& edge_list() const {
			return edge_list_;
		}



	private:
		list<ItEdge> edge_list_;

		typename list<ItEdge>::iterator find_edge(ItEdge edge) {
			for (auto e = edge_list_.begin(); e != edge_list_.end(); ++e) {
				if (edge == *e) {
					return e;
				}
			}
			assert(0);
			return edge_list_.end();
		}
	};


	/*
	 *	class Edge
	 */
	template<typename node_t, typename edge_t>
	class Edge_ {
	public:
		typedef Node_<node_t, edge_t>			Node;
		typedef typename list<Node>::iterator	ItNode;

		edge_t val;
		ItNode node0;
		ItNode node1; 

		Edge_(edge_t&& val, ItNode node0, ItNode node1) {
			this->val = val;
			this->node0 = node0;
			this->node1 = node1;
		}

		Edge_(const edge_t& val, ItNode node0, ItNode node1) {
			this->val = val;
			this->node0 = node0;
			this->node1 = node1;
		}

	};

	/*
	 *	class HalfEdge_Ptr
	 */
	template<typename node_t, typename edge_t>
	class HalfEdge_Ptr {
	public:
		typedef Node_<node_t, edge_t>			Node;
		typedef typename list<Node>::iterator	ItNode;

		edge_t* val;
		ItNode node;

		HalfEdge_Ptr(edge_t& val, ItNode node) {
			this->val = &val;
			this->node = node;
		}
	};

	/*
	 *	class HalfEdge_Value
	 */
	template<typename node_t, typename edge_t>
	class HalfEdge_Val {
	public:
		typedef Node_<node_t, edge_t>			Node;
		typedef typename list<Node>::iterator	ItNode;

		edge_t val;
		ItNode node;

		HalfEdge_Val(const edge_t& val, ItNode node) {
			this->val = val;
			this->node = node;
		}

		HalfEdge_Val(const HalfEdge_Val& he) {
			this->val = he.val;
			this->node = he.node;
		}

		HalfEdge_Val() {}
	};


	/*
	 *	class Graph
	 */
	template<typename node_t, typename edge_t>
	class Graph {
	public:
		typedef Node_<node_t, edge_t>			Node;
		typedef Edge_<node_t, edge_t>			Edge;
		typedef HalfEdge_Ptr<node_t, edge_t>	HalfEdgePtr;
		typedef HalfEdge_Val<node_t, edge_t>	HalfEdgeVal;
		typedef typename list<Node>::iterator	ItNode;
		typedef typename list<Edge>::iterator	ItEdge;


		~Graph() {}

		Graph() {}

		ItNode push_node(node_t&& val) {
			node_list_.push_front(Node(std::forward<node_t>(val)));
			return node_list_.begin();
		}

		ItNode push_node(const node_t& val) {
			node_list_.push_front(Node(val));
			return node_list_.begin();
		}

		ItEdge push_edge(edge_t&& val, ItNode node0, ItNode node1) {
			edge_list_.push_front(Edge(std::forward<edge_t>(val), node0, node1));
			ItEdge e = edge_list_.begin();
			node0->edge_list_.push_front(e);
			node1->edge_list_.push_front(e);
			return e;
		}

		ItEdge push_edge(const edge_t& val, ItNode node0, ItNode node1) {
			edge_list_.push_front(Edge(val, node0, node1));
			ItEdge e = edge_list_.begin();
			node0->edge_list_.push_front(e);
			node1->edge_list_.push_front(e);
			return e;
		}

		//ItNode find_node(const Node* node) {
		//	for (auto n = node_list_.begin(); n != node_list_.end(); ++n) {
		//		if (node == &(*n)) {
		//			return n;
		//		}
		//	}
		//	assert(0);
		//	return node_list_.end();
		//}

		//ItEdge find_edge(const Edge* edge) {
		//	for (auto e = edge_list_.begin(); e != edge_list_.end(); ++e) {
		//		if (edge == &(*e)) {
		//			return e;
		//		}
		//	}
		//	assert(0);
		//	return edge_list_.end();
		//}

		void erase_node(ItNode position) {
			Node& node = *position;

			list<ItEdge> edges(node.edge_list_);

			for (auto e = edges.begin(); e != edges.end(); ++e) {
				erase_edge(*e);
			}

			node_list_.erase(position);
		}


		void erase_edge(ItEdge position) {
			ItNode& node0 = position->node0;
			ItNode& node1 = position->node1;
			
			node0->edge_list_.erase(node0->find_edge(position));
			node1->edge_list_.erase(node1->find_edge(position));

			edge_list_.erase(position);
		}
		



		inline void vec_erase(vector<int>& v, int i) {
			int tmp = v[i];
			v[i] = v.back();
			v.back() = tmp;
			v.pop_back();
		}

		int node_index(ItNode node) {
			int c = 0;
			for (ItNode n = node_list_.begin(); n != node_list_.end(); ++n) {
				if (n == node) return c;
				++c;
			}
			return -1;
		}

		// Builds adjacency lists where each node is identified by its current position in the graph.
		// Note that the returned list is a snapshot of the current graph which is invalid after removing/adding edges/vertices.
		void adjacency(vector<list<int>>& adj) {
			adj = vector<list<int>>(node_list_.size());
			int k = 0;
			for (ItNode n = node_list_.begin(); n != node_list_.end(); ++n) {
				for (auto e = n->edge_list_.begin(); e != n->edge_list_.end(); ++e) {
					if ((*e)->node0 != n) {
						adj[k].push_front(node_index((*e)->node0));
					} else {
						adj[k].push_front(node_index((*e)->node1));
					}
				}
				++k;
			}
		}


		void connected_components(list<list<ItNode>>& cc) {
			vector<list<int>> adj;
			adjacency(adj);

			vector<ItNode> nodes_vector(adj.size());
			vector<int>	visited(adj.size());
			unordered_set<int> all_node_ids;

			//list<list<int>> ccc;

			int c = 0;
			for (ItNode n = node_list_.begin(); n != node_list_.end(); ++n) {
				nodes_vector[c] = n;
				visited[c] = 0;
				all_node_ids.insert(c);
				++c;
			}

			while (!all_node_ids.empty()) {

				//ccc.push_front(list<int>());
				cc.push_front(list<ItNode>());

				stack<int> node_ids;

				node_ids.push(*all_node_ids.begin());

				while (!node_ids.empty()) {

					int id = node_ids.top();
					node_ids.pop();

					if (visited[id] == 0) { // not yet visited
						visited[id] = 1;

						//ccc.front().push_front(id);
						cc.front().push_front(nodes_vector[id]);

						all_node_ids.erase(id);

						// push not visited neighbours
						for (auto i = adj[id].begin(); i != adj[id].end(); ++i) {
							if (visited[*i] == 0) {
								node_ids.push(*i);
							}
						}
					}
				}

			}


			////////////////////

			//cout << "############" << endl;
			//for (auto i = ccc.begin(); i != ccc.end(); ++i) {
			//	for (auto j = i->begin(); j != i->end(); ++j) {
			//		cout << *j;
			//	}
			//	cout << endl;
			//}

		}



		// Merge all specified nodes to one node with given value.
		// The new node has all the edges of the old nodes.
		ItNode merge_nodes(const list<ItNode>& nodes, const node_t& val) {
			ItNode new_node = push_node(val);

			for (auto in = nodes.begin(); in != nodes.end(); ++in) {
				for (auto e = (*in)->edge_list_.begin(); e != (*in)->edge_list_.end(); ++e) {
					// connect the input nodes edges to the new node
					if ((*e)->node0 == *in) {
						(*e)->node0 = new_node;
					} else {
						(*e)->node1 = new_node;
					}

					if ((*e)->node0 != (*e)->node1) {	// if equal edge is already in new_node's edge_list
						new_node->edge_list_.push_front(*e);
					}
				}
			}

			// remove all input nodes, note that erase_node would destroy the connection to the new node
			for (auto in = nodes.begin(); in != nodes.end(); ++in) {
				(*in)->edge_list_.clear();
				node_list_.erase(*in);
			}

			// remove cycles on new_node
			auto e = new_node->edge_list_.begin();
			while (e != new_node->edge_list_.end()) {
				if ((*e)->node0 == (*e)->node1) {
					edge_list_.erase(*e);
					e = new_node->edge_list_.erase(e);
					continue;
				}
				++e;
			}

			return new_node;
		}


		// DEPRECATED
		// Merge all specified nodes to one node with given value.
		// The new node has all the edges of the old nodes.
		ItNode merge_nodes2(const list<ItNode>& nodes, const node_t& val) {

			list<HalfEdgePtr> half_edge;

			// push all nodes which are connected to input nodes to half_edge, including the input nodes itself
			for (auto in = nodes.begin(); in != nodes.end(); ++in) {
				for (auto e = (*in)->edge_list_.begin(); e != (*in)->edge_list_.end(); ++e) {
					half_edge.push_front(HalfEdgePtr((*e)->val, (*e)->node0));
					half_edge.push_front(HalfEdgePtr((*e)->val, (*e)->node1));
				}
			}

			// remove all instances of input nodes from half_edge
			auto he = half_edge.begin();
			while (he != half_edge.end()) {
				for (auto in = nodes.begin(); in != nodes.end(); ++in) {
					if (he->node == *in) {
						he = half_edge.erase(he);
						goto break_continue;
					}
				}
				++he;
				break_continue:
				(void)0;
			}

			// build the new node
			auto new_node = push_node(val);
			for (auto n = half_edge.begin(); n!= half_edge.end(); ++n) {
				push_edge(*n->val, n->node, new_node);
			}

			// remove all input nodes
			for (auto in = nodes.begin(); in != nodes.end(); ++in) {
				erase_node(*in);
			}

			return new_node;
		}



		inline list<Node>& node_list() {
			return node_list_;
		}

		inline list<Edge>& edge_list() {
			return edge_list_;
		}

		inline const list<Node>& node_list() const {
			return node_list_;
		}

		inline const list<Edge>& edge_list() const {
			return edge_list_;
		}

	private:
		list<Node> node_list_;
		list<Edge> edge_list_;
	};


}



/*
 * ostream operator<< overload
 */
template <typename node_t, typename edge_t>
std::ostream& operator<<(std::ostream& out, const pcv::Graph<node_t, edge_t>& g) {
	if (g.node_list().empty()) {
		out << "[]";
		return out;
	}
	
	for (auto node = g.node_list().begin(); node != g.node_list().end(); ++node) {
		out << node->val << endl;
		for (auto edge = node->edge_list().begin(); edge != node->edge_list().end(); ++edge) {
			out << "  " << (*edge)->node0->val << " <- " << (*edge)->val << " -> " << (*edge)->node1->val << endl;
		}
	}

	return out;
}

void test_graph();


#endif