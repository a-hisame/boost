// A*アルゴリズムを用いて最短経路を求める
// ただし、解を発見した時点で処理を打ち切る

#include<iostream>
#include<utility>
#include<vector>
#include<algorithm>
#include<cmath>

#include<boost/foreach.hpp>
#include<boost/lambda/lambda.hpp>
#include<boost/graph/properties.hpp>
#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/astar_search.hpp>

// euclidean distance heuristic
template <class Graph, class Vertex, class CostType>
class distance_heuristic : public boost::astar_heuristic<Graph, CostType>
{
  typedef typename 
    boost::property_map<Graph, boost::vertex_distance_t>::const_type PosMap;
public:
  distance_heuristic(PosMap pos_map, Vertex goal) : 
	  m_pos_map(pos_map), m_goal(goal) {}
  CostType operator()(Vertex u)
  {
    auto p1 = m_pos_map[m_goal];
    auto p2 = m_pos_map[u];
    CostType dx = abs(p1.first - p2.first);
    CostType dy = abs(p1.second - p2.second);
    return dx + dy; 
  }
private:
  PosMap m_pos_map;
  Vertex m_goal;
};

struct found_goal {};

template <class Vertex>
class goal_visitor : public boost::default_astar_visitor {
public:
  goal_visitor(Vertex goal) : m_goal(goal), seek_count(0) {}

  template <class Graph>
  void discover_vertex(Vertex u, const Graph & g) {
    ++seek_count;
    if(u == m_goal) { 
//      std::cout << "num_vertices = " << boost::num_vertices(g)
//	      << ", seek_count = " << seek_count << std::endl;
      throw found_goal();
    }
  }
private:
  Vertex m_goal;
  int seek_count;
};

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

// 見つけた時点で打ち切るためには、
// visitorでthrowをして強制的に打ち切り、
// catchして処理を行う。(by boostサンプル)
std::list<Pos> get_shortest_path(const Graph& g) {
  auto ps = seek_pos(g);
  std::vector<int> dist(boost::num_vertices(g));
  std::vector<Vertex> parents(boost::num_vertices(g));
  try {
    boost::astar_search(g, ps.first, 
      distance_heuristic<Graph, Vertex, int>(
        boost::get(boost::vertex_distance_t(), g), ps.second),
        boost::predecessor_map(&parents[0]).
	visitor(goal_visitor<Vertex>(ps.second)));
  }catch(found_goal fg) {
    // found goal vertex.
    auto pos_map = boost::get(boost::vertex_distance_t(), g);
    std::list<Pos> result;
    for(Vertex v = ps.second; v != ps.first; v = parents[v]) {
      result.push_front(pos_map[v]); 
    }
    result.push_front(pos_map[ps.first]);
    return result;
  }
  // not found!
  return std::list<Pos>();
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

