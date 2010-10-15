// Using Dijkstra Algorithm Sample

#include<iostream>
#include<utility>
#include<vector>
#include<algorithm>

#include<boost/foreach.hpp>
#include<boost/graph/properties.hpp>
#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/dijkstra_shortest_paths.hpp>

typedef boost::property<boost::edge_weight_t, int> EdgeProperty;

typedef boost::adjacency_list<
  boost::vecS, boost::vecS, boost::undirectedS,
  boost::no_property, EdgeProperty> Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef std::pair<int, int> Edge;

Graph make_graph() {
  std::vector<Edge> es = {
    Edge(0, 1), Edge(0, 2), Edge(0, 5), Edge(0, 8), 
    Edge(1, 2), Edge(1, 4), Edge(1, 7), Edge(1, 9),
    Edge(2, 3), Edge(2, 6), 
    Edge(3, 5), Edge(3, 8), Edge(3, 9),
    Edge(4, 5), Edge(4, 7),
    Edge(5, 8),
    Edge(8, 9)
  };
  std::vector<int> weights = {
     3, 10, 7, 4, 
     4, 5, 9, 3, 
     9, 2,
     1, 2, 3,
     2, 5,
     1,
     4
  };

  // Constractor Parameters:
  // adjacency_list(EdgeIterator first, EdgeIterator last,
  //                EdgePropertyIterator ep_iter,
  //                vertices_size_type n,
  //                edges_size_type m = 0,
  //                const GraphProperty& p = GraphProperty())
  Graph g(es.begin(), es.end(), weights.begin(), 10); 

  return g;
}

void dijkstra(int start_index, const Graph& g) {
  // boost::graph_traits<Graph>::vertex_descriptor
  auto s = boost::vertex(start_index, g);
  std::vector<int> result(boost::num_vertices(g));
  std::vector<Vertex> parent(boost::num_vertices(g));

  // std::vector<boost::graph_traits<Graph>::vertex_descriptor> vertex;
  // 結果の受け取り方が第三引数
  // distance_map(距離) / predecessor_map(祖先=そのノードへたどり着く直前)
  dijkstra_shortest_paths(g, s,
	boost::distance_map(&result[0]).predecessor_map(&parent[0]));

  std::cout << "Shortest Path: " << std::endl;
  BOOST_FOREACH(auto v, boost::vertices(g)) {
    std::cout << "d(" << s << ", " << v << ") = " 
	    << result[v] << " "
	    << "parent = " << parent[v] << std::endl;
  }
  std::cout << std::endl;

}

void print_min_path(int begin, int end, const Graph& g) {
  
  auto s = boost::vertex(begin, g);
  std::vector<int> result(boost::num_vertices(g));
  std::vector<Vertex> parent(boost::num_vertices(g));

  dijkstra_shortest_paths(g, s,
	boost::distance_map(&result[0]).
	predecessor_map(&parent[0]));

  Vertex v = boost::vertex(end, g);
  std::cout << "Paths(" << begin << "->" << end << ") : ";
  while(v != s) {
    std::cout << v << "<-";
    v = parent[v];
  }
  std::cout << begin << std::endl;
}

void print_graph(const Graph& g) {

  // boost::property_map<Graph, boost::edge_weight_t>::const_type
  auto edge_values = boost::get(boost::edge_weight_t(), g);
  
  std::cout << "All Vertices: ";
  BOOST_FOREACH(auto e, boost::vertices(g)) {
    std::cout <<  e << " ";
  }
  std::cout << std::endl;

  std::cout << "All Edges: ";
  BOOST_FOREACH(auto e, boost::edges(g)) {
    std::cout << "edge [(" << 
      boost::source(e, g) << ", " <<
      boost::target(e, g) << ") " <<
      "weight = " << edge_values[e] << "] ";
  }
  std::cout << std::endl;

  dijkstra(0, g);
  for(int i = 1; i < 10; i++) {
    print_min_path(0, i, g);
  }
}


int main(void) {
  Graph g = make_graph();
  print_graph(g); 
  return 0;
}

