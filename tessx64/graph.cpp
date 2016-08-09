#include "graph.h"

#include <iostream>

using std::cout;
using std::endl;



typedef pcv::Graph<int, float>	MyGraph;
typedef MyGraph::Node			MyNode;
typedef list<MyNode>::iterator	ItNode;


void test_graph() {
	
	MyGraph graph;


	auto node0 = graph.push_node(0);
	auto node1 = graph.push_node(1);
	auto node2 = graph.push_node(2);
	auto node3 = graph.push_node(3);
	auto node4 = graph.push_node(4);
	auto node5 = graph.push_node(5);

	auto edge0 = graph.push_edge(0.2f, node0, node1);
	auto edge1 = graph.push_edge(-1.5f, node1, node2);
	auto edge2 = graph.push_edge(0.3f, node1, node3);
	auto edge3 = graph.push_edge(3.1415f, node0, node3);
	auto edge4 = graph.push_edge(2.5f, node4, node5);

	cout << graph << endl;
	cout << graph.node_list().size() << endl;
	cout << graph.edge_list().size() << endl;
	cout << "--------------------" << endl;

	list<list<ItNode>> cc;
	graph.connected_components(cc);

	for (auto i = cc.begin(); i != cc.end(); ++i) {
		for (auto j = i->begin(); j != i->end(); ++j) {
			cout << (*j)->val << " ";
		}
		cout << endl;
	}


	cout << "-----------------" << endl;

	//graph.erase_node(node0);
	//graph.erase_edge(edge3);
	list<ItNode> node_iter;
	node_iter.push_front(node0);
	node_iter.push_front(node4);
	graph.merge_nodes(node_iter, 100);

	//ItNode itn = graph.node_list().begin();
	//graph.node_list().begin()->val = 100;
	

	
	cout << graph << endl;
	cout << graph.node_list().size() << endl;
	cout << graph.edge_list().size() << endl;

	cout << "-----------------" << endl;




	vector<list<int>> adj;

	graph.adjacency(adj);

	for (int k = 0; k < adj.size(); ++k) {
		cout << k << endl;
		for (auto i = adj[k].begin(); i != adj[k].end(); ++i) {
			cout << "  " << *i << endl;
		}
	}
}