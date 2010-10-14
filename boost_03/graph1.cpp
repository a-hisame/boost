#include<iostream>
#include<utility>
#include<vector>
#include<algorithm>

#include<boost/foreach.hpp>
#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/properties.hpp>

using namespace std;
using namespace boost;

// multi-properties are defined by nested such as following.
typedef property<vertex_name_t, string, 
	property<vertex_distance_t, double>> VertexProperty;

// EdgeList, VertexList, Directed, 
// VertexProperties
typedef adjacency_list<vecS, vecS,
	undirectedS, VertexProperty> Graph;
typedef pair<int, int> Edge;

Graph make_graph() {
  Graph g;
  vector<Edge> es = {
    Edge(0, 1), Edge(0, 2), 
    Edge(2, 3)
  };
  for_each(es.begin(), es.end(), [&g](Edge e) {
    add_edge(e.first, e.second, g);
  });

//  property_map<Graph, vertex_name_t>::type 
  auto name_map = boost::get(vertex_name_t(), g);
  boost::put(name_map, 0, "Root");
  boost::put(name_map, 1, "Child1");
  boost::put(name_map, 2, "Child2");
  boost::put(name_map, 3, "Child3");

//  property_map<Graph, vertex_distance_t>::type 
//  indexer accessable
  auto value_map = get(vertex_distance_t(), g);
  value_map[0] = 3.0;
  value_map[1] = 3.5;
  value_map[2] = 4;
  value_map[3] = -2.0;

  return g;
}

void print_graph(const Graph& g) {
//  property_map<Graph, vertex_name_t>::const_type
  auto name_map = boost::get(vertex_name_t(), g);
//  property_map<Graph, vertex_distance_t>::const_type
  auto value_map = boost::get(vertex_distance_t(), g);

// you can use 'boost::get(map, index)' too.
  cout << "All Vertices: ";
  BOOST_FOREACH(auto e, vertices(g)) {
    cout << "(" << name_map[e] <<
	    ", " << value_map[e] << ") ";
  }
  cout << endl;

  cout << "All Edges: ";
  BOOST_FOREACH(auto e, edges(g)) {
    cout << "(" << 
      name_map[source(e, g)] << ", " <<
      name_map[target(e, g)] << ") ";
  }
  cout << endl;
}

int main(void) {
  Graph g = make_graph();
  print_graph(g); 
  return 0;
}

