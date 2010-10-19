#include<iostream>
#include<utility>
#include<vector>
#include<algorithm>

#include<boost/foreach.hpp>
#include<boost/graph/adjacency_list.hpp>


typedef boost::adjacency_list<
  boost::vecS, boost::vecS,
  boost::undirectedS> Graph;

typedef std::pair<int, int> Edge;

Graph make_graph() {
  std::vector<Edge> es = {
    Edge(0, 2), Edge(0, 3),
    Edge(1, 2), Edge(2, 3)
  };
  return Graph(es.begin(), es.end(), 4); 
}

Graph make_graph_by_addtion() {
  std::vector<Edge> es = {
    Edge(0, 2), Edge(0, 3),
    Edge(1, 2), Edge(2, 3)
  };
  Graph g;
  std::for_each(es.begin(), es.end(), [&g](const Edge& e) {
    boost::add_edge(e.first, e.second, g);
  });
  return g;
}

int main(void) {
  Graph g = make_graph();
  return 0;
}

