// find shortest_path problem:
// source: http://okajima.air-nifty.com/b/2010/01/post-abc6.html
// author: a-hisame

#include<iostream>
#include<utility>
#include<vector>
#include<algorithm>
#include<cmath>

#include<boost/foreach.hpp>
#include<boost/lambda/lambda.hpp>
#include<boost/graph/properties.hpp>
#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/dijkstra_shortest_paths.hpp>

typedef std::pair<int, int> Pos;

typedef boost::property<boost::vertex_distance_t, Pos,
	boost::property<boost::vertex_color_t, int> > VertexProperty;

typedef boost::property<boost::edge_weight_t, int> EdgeProperty;

typedef boost::adjacency_list<
  boost::setS, boost::vecS, boost::undirectedS,
  VertexProperty, EdgeProperty> Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;

enum field {
  None, Start, Goal
};

std::vector<std::string> get_input() {
  std::vector<std::string> in;
  std::string str;
  while(std::getline(std::cin, str)) {
    in.push_back(str);
  }
  return in;
}

Graph make_graph(std::vector<std::string> in) {
  
  Graph g;
  auto pos_map = boost::get(boost::vertex_distance_t(), g);
  auto color_map = boost::get(boost::vertex_color_t(), g);
  for(int i = 0; i < in.size(); ++i) {
    std::string s = in[i];
    for(int j = 0; j < s.size(); ++j) {
      if(s[j] == '*') continue;
      Vertex v = boost::add_vertex(g);
      pos_map[v] = Pos(j, i); // (x, y) style.
      color_map[v] = (s[j] == 'S') ? Start :
                     (s[j] == 'G') ? Goal : None;
    }
  }

  auto edge_map = boost::get(boost::edge_weight_t(), g);
  BOOST_FOREACH(auto s, vertices(g)) {
    BOOST_FOREACH(auto t, vertices(g)) {
      auto p1 = pos_map[s];
      auto p2 = pos_map[t];
      if( abs(p1.first-p2.first) + abs(p1.second-p2.second) == 1 ) {
	std::pair<Edge, bool> e = boost::add_edge(s, t, g);
	if(e.second) {
	  edge_map[e.first] = 1;
	}
      }
    }
  }
  
  return g;
}

// return pair<start, goal>
std::pair<Vertex, Vertex> seek_pos(const Graph& g) {
  std::pair<Vertex, Vertex> res;
  auto color_map = boost::get(boost::vertex_color_t(), g);
  BOOST_FOREACH(auto v, boost::vertices(g)) {
    if(color_map[v] == Start) res.first = v;
    if(color_map[v] == Goal) res.second = v;
  }
  return res;
}

std::list<Pos> get_shortest_path(const Graph& g) {
  auto ps = seek_pos(g);
  std::vector<int> dist(boost::num_vertices(g));
  std::vector<Vertex> parents(boost::num_vertices(g));
  boost::dijkstra_shortest_paths(g, ps.first,
	boost::predecessor_map(&parents[0]).
	distance_map(&dist[0]));

  auto pos_map = boost::get(boost::vertex_distance_t(), g);
  std::list<Pos> result;

  for(Vertex v = ps.second; v != ps.first; v = parents[v]) {
    result.push_front(pos_map[v]);
  }
  result.push_front(pos_map[ps.first]);

  return result;
}

bool exists_pos(const std::list<Pos>& path, Pos dest) {
  return std::find(path.begin(), path.end(), dest) != path.end();
}

void print_path(const Graph& g, const std::vector<std::string>& in) {
  auto path = get_shortest_path(g);

  for(int i = 0; i < in.size(); ++i) {
    auto s = in[i];
    for(int j = 0; j < s.size(); ++j) {
      if( s[j] == ' ' && exists_pos(path, Pos(j,i)) ) {
        std::cout << "$";
      } else {
        std::cout << s[j];
      }
    }
    std::cout << std::endl;
  }

  std::for_each(path.begin(), path.end(), [](Pos p) {
    std::cout << "(" << p.first << "," 
    	<< p.second << ") -> ";
  });
  std::cout << std::endl;
}

int main(void) {
  auto in = get_input();
  Graph g = make_graph(in);
  print_path(g, in);
  return 0;
}

