#include <iostream>
#include <vector>
#include <fstream>
#include <set>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/integer_traits.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/simple_point.hpp>
#include <boost/graph/metric_tsp_approx.hpp>

using namespace boost;
using namespace std;

// simple_pointはboost/graph/simple_point.hppで定義されるx,yでアクセスできるペア構造体
typedef vector<simple_point<double> > PosVector;
typedef adjacency_matrix<undirectedS, no_property,
    property <edge_weight_t, double> > Graph;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::edge_descriptor Edge;
typedef vector<Vertex> Container;
typedef property_map<Graph, edge_weight_t>::type WeightMap;
typedef property_map<Graph, vertex_index_t>::type VertexMap;

void usage(char* argv[]) {
	cout << "usage: " << argv[0] << " input.txt" << endl
	     << "input.txt has point on line. for example" << endl
	     << "10 30" << endl;
    return;
}

void print_graph(Graph& g, PosVector& pos) {

	WeightMap wmap(get(edge_weight, g));
	VertexMap vmap(get(vertex_index, g));

	cout << "All Vertices: ";
    BOOST_FOREACH(auto v, vertices(g)) {
	    auto p = pos[vmap[v]];
		cout << "[" << vmap[v] <<  ":(" << p.x << "," << p.y << ")] ";
	}
	cout << endl << endl;
	
	cout << "All Edges: ";
	BOOST_FOREACH(auto e, edges(g)) {
		cout << "(" << vmap[target(e, g)] << "-" << vmap[source(e, g)] 
			<< ": " << wmap[e] << ") ";
	}
	cout << endl;

}

// ファイル入力 -> 座標変換
PosVector get_input(char* filename) {
	ifstream fin(filename);
    if (!fin) {
		cout << "ifstream error." << endl;
		exit(1);
    }

	PosVector vs;
	string line;
    // 取得方法はサンプルをほぼ流用
	while (getline(fin, line)) {
		simple_point<double> vertex;

		size_t idx(line.find(","));
		string xStr(line.substr(0, idx));
		string yStr(line.substr(idx + 1, line.size() - idx));

		vertex.x = lexical_cast<double>(xStr);
		vertex.y = lexical_cast<double>(yStr);
		vs.push_back(vertex); 
	}
    fin.close();
	return vs;
}

// 座標 -> 完全グラフ化（距離込)
Graph make_graph(PosVector vs) {
    // 隣接行列なので、頂点数のみでエッジなしのグラフが構成される
	Graph g(vs.size());
	WeightMap wmap(get(edge_weight, g));
	VertexMap vmap(get(vertex_index, g));

    // pair<頂点iter, 頂点iter>
    auto verts = vertices(g);
    for (auto src = verts.first; src != verts.second; src++) {
        for (auto dest = src; dest != verts.second; dest++) {
		    // 自己ループのエッジはなし
            if (dest == src) continue;
			double dx = static_cast<double>(vs[vmap[*src]].x - vs[vmap[*dest]].x);
			double dy = static_cast<double>(vs[vmap[*src]].y - vs[vmap[*dest]].y);
			double weight = sqrt(pow(dx, 2.0) + pow(dy, 2.0));

			// 重み付き辺の作成
			Edge e;
			bool inserted;
			boost::tie(e, inserted) = add_edge(*src, *dest, g);
			wmap[e] = weight;
		}
    }
	return g;
}

void my_tsp_tour(Graph& g, PosVector& pos) {
    vector<Vertex> res; 
	VertexMap vmap(get(vertex_index, g));
	WeightMap wmap(get(edge_weight, g));

    // PATH (ONLY)
    // metric_tsp_approx_tour(g, back_inserter(res)); 
	
    // PATH with distance
	double dist = 0.0;
	res.clear();
	metric_tsp_approx(g, make_tsp_tour_len_visitor(
		g, back_inserter(res), dist, wmap));
	// result output
	cout << "PATH DISTANCE: " << dist << endl;
	cout << "PATH: ";
	BOOST_FOREACH(auto v, res) {
      cout << vmap[v] << " -> ";
	}
	cout << endl << endl;

}

int main(int argc, char* argv[]) {
    // input check;
	if(argc < 2) {
		usage(argv);
		return 0;
	}
	
	auto in = get_input(argv[1]);
	Graph g = make_graph(in);
	print_graph(g, in);
	my_tsp_tour(g, in);

	return 0;
}

