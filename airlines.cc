#include <iostream>
#include <vector>
#include <queue>
#include <stack>

#include <string>
#include <sstream>
#include <fstream>
#include <chrono>

using namespace std;

// *** Data structures ***

struct Flight {
	int orig, dest, dept, arr;

	Flight(int orig, int dest, int dept, int arr) :
		orig(orig), dest(dest), dept(dept), arr(arr) {}
};

struct Edge {
	int to, cap, flow;

	Edge(int to, int cap, int flow) :
		to(to), cap(cap), flow(flow) {}
};

typedef vector<int> VI;
typedef vector<VI> MI;
typedef vector<vector<Edge>> Graph;
typedef vector< vector <pair<int,int> > > IImat;		// Següent node, capacitat aresta.
typedef vector< pair<int,int> > IIvec;					// Següent node, capacitat aresta.

// *** Global variables ***

// source (supply k), sink (demmand k), Super Source, Super Sink
const int s=2, t=3, S=0, T=1;

// *** Graph building ***

void parseArgs(int argc, char *argv[], int& N, int& X, int& A) {
	if (argc != 4) {
		cout << "Usage: airlines <airports> <version> <algorithm>" << endl;
		exit(1);
	}
	std::string arg1 = argv[1];
	std::string arg2 = argv[2];
	std::string arg3 = argv[3];
	N = atoi(arg1.c_str());
	A = atoi(arg3.c_str());
	if (arg2 == "1") X = 1;
}

void readFlights(vector<Flight>& flights, MI& airports, int& X) {
	int o, d, h1, h2, i=0;
	while (cin >> o >> d >> h1 >> h2) {
		flights.push_back(Flight(o,d,h1,h2));
		airports[o].push_back(i++);
	}
	if (X == -1) X = flights.size();
}

void readFlightsFile(string s, vector<Flight>& flights, MI& airports, int& X) {
	ifstream myfile;
	myfile.open(s.c_str());
	int o, d, h1, h2, i=0;
	while (myfile >> o >> d >> h1 >> h2) {
		flights.push_back(Flight(o,d,h1,h2));
		airports[o].push_back(i++);
	}
	if (X == -1) X = flights.size();
	myfile.close();
}

void buildGraph(Graph& g, vector<Flight>& flights, MI& airports, int k, int X) {
	g[S].push_back(Edge(s,k,0));
	g[t].push_back(Edge(T,k,0));
	int i = 4;
	for (Flight f : flights) {
		g[i].push_back(Edge(T,1,0));
		g[i+1].push_back(Edge(t,1,0));
		g[S].push_back(Edge(i+1,1,0));
		for (int j = 0; j < X; j++) {
			if (j) g[i].push_back(Edge(i+1,1,0));
			g[s].push_back(Edge(i,1,0));
		}
		for (auto a : airports[f.dest]) {
			if (flights[a].dept - f.arr >= 15) {
				for (int j = 0; j < X; j++) {
					g[i+1].push_back(Edge(4+2*a,1,0));
				}
			}
		}
		i += 2;
	}
}

void printGraph(Graph& g) {
	//cout << "N: " << N << endl;
	//cout << "X: " << X << endl;
	//cout << "Flights: " << flights.size() << endl;
	//cout << "Airports: " << airports.size() << endl;
	cout << "Super Source " << S << endl;
	for (auto x : g[S]) {
		cout << "\tTo " << x.to << " c " << x.cap << " f " << x.flow << endl;
	}
	cout << "Super Sink " << T << endl;
	for (auto x : g[T]) {
		cout << "\tTo " << x.to << " c " << x.cap << " f " << x.flow << endl;
	}
	cout << "Source " << s << endl;
	for (auto x : g[s]) {
		cout << "\tTo " << x.to << " c " << x.cap << " f " << x.flow << endl;
	}
	cout << "Sink " << t << endl;
	for (auto x : g[t]) {
		cout << "\tTo " << x.to << " c " << x.cap << " f " << x.flow << endl;
	}
	for (int i = 4; i < (int)g.size(); i++) {
		cout << "Node " << i << endl;
		for (auto x : g[i]) {
			cout << "\tTo " << x.to << " c " << x.cap << " f " << x.flow << endl;
		}
	}
}

// *** MaxFlow general functions ***
void buildResidual(const Graph & g, Graph & r) {
	for (int i = 0; i < (int)g.size(); i++) {
		for (int j = 0; j < (int)g[i].size(); j++) {
			// Edges start with all the flow available.
			r[i].push_back((Edge(g[i][j].to,g[i][j].cap, g[i][j].cap)));
		}
	}
}

void residualToGraph(const Graph & r, Graph & g) {
	for (int i = 0; i < (int) r.size(); i++) {
		for (int j = 0; j < (int) r[i].size(); j++) {
			// If capacity is equal to -1 the edge is not an edge of the genuine graph.
			if (r[i][j].cap != -1) g[i].push_back((Edge(r[i][j].to,r[i][j].cap, r[i][j].cap - r[i][j].flow)));
		}
	}
}

int findPosition(Graph& r, int ini, int end) {
	for (int i = 0; i < (int)r[ini].size(); i++) {
		if (r[ini][i].to == end) return i;
	}
	// The searched edge doesn't exist, now we create it.
	r[ini].push_back(Edge(end,-1,0));
	return r[ini].size()-1;
}

void augment(Graph& r, const VI& path, int bottleneck) {
	int vAct = T;
	while (vAct != S) {
		int vIni = path[vAct];
		int p1 = findPosition(r,vAct,vIni);
		r[vAct][p1].flow += bottleneck;
		int p2 = findPosition(r,vIni,vAct);
		r[vIni][p2].flow -= bottleneck;

		vAct = path[vAct];
	}
}

// *** Edmonds-Karp MaxFlow algorithm functions ***

int findPath(const Graph& r, VI& path) {
	int bottleneck = 0;
	path = VI(r.size(),-1);
	VI cap(r.size(),0);
	queue<int> vertexs;
	vertexs.push(S);
	cap[S] = 5000000;
	path[S] = -2;
	while (not vertexs.empty()) {
		int vIni = vertexs.front();
		vertexs.pop();
		for (auto e : r[vIni]) {
			int vAct = e.to;
			int capR = e.flow;
			if (capR > 0 and path[vAct] == -1) {
				path[vAct] = vIni;
				cap[vAct] = min(cap[vIni],capR);
				if (vAct == T) return cap[T];
				vertexs.push(vAct);
			}
		}
	}
	return bottleneck;
}

int edmondsKarp(Graph & g) {
	Graph r(g.size());
	buildResidual(g,r);
	VI path;
	int bottleneck, maxflow=0;
	while((bottleneck = findPath(r,path)) > 0) {
		augment(r,path,bottleneck);
		maxflow += bottleneck;
	}
	Graph g1(g.size());
	// Now we create the max flow graph from the residual graph.
	residualToGraph(r, g1);
	g = g1;
	return maxflow;
}

// *** Dinic MaxFlow algorithm functions ***

void delEdge(Graph& g, int vAct, int vSeg) {
	int i = -1;
	// It will find the position of vSeg node into the g graph.
	while (++i < (int) g[vAct].size()-1 and g[vAct][i].to != vSeg);
	// Now we delete the position.
	if (g[vAct][i].to == vSeg) g[vAct].erase(g[vAct].begin() + i);
}

void delPre(MI & pre, int vAct, int vSeg) {
	int i = -1;
	// It will find the position of vAct node into the pre matrix.
	while(++i < (int) pre[vSeg].size()-1 and pre[vSeg][i] != vAct);
	// Now we delete the position.
	if(pre[vSeg][i] == vAct) pre[vSeg].erase(pre[vSeg].begin() + i);
}

//This function is going to delete all invalid edges.
void delInvalid(Graph& g, MI& pre, int vAct, int vSeg) {
	delEdge(g, vAct, vSeg);
	delPre(pre, vAct, vSeg);
	if ((int) g[vAct].size() == 0 and vAct != T) {
		for(int vAnt : pre[vAct]) {
			delInvalid(g, pre, vAnt, vAct);
		}
	}
}

int dinicBfs(Graph& g) {
	VI dis(g.size(),-1);
	dis[S] = 0;
	MI pre(g.size());
	queue<int> q;
	q.push(S);
	while (not q.empty()) {
		int vIni = q.front();
		q.pop();
		VI del;
		for (int i = (int) g[vIni].size()-1; i >= 0; i--) {
			Edge e = g[vIni][i];
			int vAct = e.to;
			if (dis[vAct] == -1) {
				// It's the first time we have arrived to the node
				dis[vAct] = dis[vIni] + 1;
				pre[vAct].push_back(vIni);
				q.push(vAct);
			} else if (dis[vAct] == dis[vIni] + 1) {
				// We have arrived to the node again with the minimum distance from initial node.
				pre[vAct].push_back(vIni);
			} else {
				// We have arrived to the node again, and the distance is not the minimum.
				g[vIni].erase(g[vIni].begin() + i);
			}
		}
		if (g[vIni].empty() and vIni != T) for (int vAnt : pre[vIni]) delInvalid(g, pre, vAnt, vIni);
	}
	return dis[T];
}

bool dinicDfs(Graph& g, VI& path, VI& bott, int v) {
	bool trobat = false;
	if (v == T) trobat = true;
	else {
		int i = 0;
		while (i < (int) g[v].size() and not trobat) {
			Edge e = g[v][i];
			int u = e.to;
			if (path[u] == -1) {
				path[u] = v;
				bott[u] = min(e.flow, bott[v]);
				trobat = dinicDfs(g, path,bott, u);
			}
			i++;
		}
	}
	return trobat;
}

// This function makes a copy of the g graph, the result doesn't have any edge with flow 0.
void copyGraph(const Graph& g, Graph& aux) {
	for (int i = 0; i < (int) g.size();i++) {
		for (int j = 0; j < (int) g[i].size(); j++) {
			Edge e = g[i][j];
			if (e.flow != 0) aux[i].push_back(Edge(e.to, e.cap, e.flow));
		}
	}
}

//This function will delete all edges from tha path that have flow 0.
void removeEdges(Graph& g, const VI& path) {
	int vAct = T;
	while (vAct != S) {
		int vIni = path[vAct];
		int i = -1;
		while (g[vIni][++i].to != vAct);
		if (g[vIni][i].flow == 0) g[vIni].erase(g[vIni].begin() + i);

		vAct = vIni;
	}
}

int dinic(Graph & g) {
	Graph r(g.size());
	buildResidual(g,r);
	Graph aux = r;
	int maxflow=0;
	while(dinicBfs(aux) > 0) {
		VI path(aux.size(), -1);
		path[S] = -2;
		VI bott(aux.size(), 0);
		bott[S] = 5000000;
		while (dinicDfs(aux, path, bott, S)){
			augment(aux,path,bott[T]);
			removeEdges(aux, path);
			augment(r,path,bott[T]);
			maxflow += bott[T];

			path = VI(r.size(), -1);
			path[S] = -2;
			bott = VI(r.size(), 0);
			bott[S] = 5000000; 
		}
		aux = Graph(g.size());
		copyGraph(r, aux);	
	}
	Graph g1(g.size());
	residualToGraph(r, g1);
	g = g1;
	return maxflow;
}

// *** Main ***

void driverSchedule(Graph& g, vector<bool>& vis, int u) {
	VI aux;
	int x = u;
	while(x != t) {
		if (not vis[(x-4)/2]) {
			vis[(x-4)/2] = true;
			aux.push_back((x-4)/2+1);
		}
		for (int j = 0; j < (int)g[x+1].size(); j++) {
			if (g[x+1][j].flow) {
				g[x+1][j].flow--;
				x = g[x+1][j].to;
				break;
			}
		}
	}
	for (int i = 0; i < (int)aux.size(); i++) {
		if (i) cout << " ";
		cout << aux[i];
	}
	cout << endl;
}

void printSchedule(Graph& g) {
	vector<bool> visit(((g.size()-4)/2),false);
	for (auto e : g[s]) {
		if (e.flow) driverSchedule(g,visit,e.to);
	}
}

void solve(int N, int X, int A) {
	// All the flights with it's info
	vector<Flight> flights;
	// Flights of each airport
	vector<vector<int>> airports(N);
	readFlights(flights,airports,X);

	// number of pilots to serve all flights
	int k=0;
	
	Graph ans;
	// Binary search to find the minimum k
	int l = 0, r = flights.size();
	while (l <= r) {
		int m = (l+r)/2;

		// Graph that represents the network flow
		Graph g(flights.size()*2+4);
		buildGraph(g,flights,airports,m,X);

		int flow = 0;
		if (A == 1) flow = edmondsKarp(g);
		else if (A == 2) flow = dinic(g);
		if (flow < m + (int) flights.size()) l = m + 1;
		else {
			k = m;
			ans = g;
			r = m - 1;
		}
	}
	cout << k << endl;
	printSchedule(ans);
}

int solveFile(string file, int N, int X, int A) {
	string s = "./Benchmark/" + file;

	// All the flights with it's info
	vector<Flight> flights;
	// Flights of each airport
	vector<vector<int>> airports(N);
	readFlightsFile(s,flights,airports,X);

	// number of pilots to serve all flights
	int k=0;
	// Graph that represents the network flow
	Graph g(flights.size()*2+4);
	buildGraph(g,flights,airports,k,X);
	
	// Binary search to find the minimum k
	int l = 0, r = flights.size();
	while (l <= r) {
		int m = (l+r)/2;
		// Graph that represents the network flow
		Graph g(flights.size()*2+4);
		buildGraph(g,flights,airports,m,X);
		int flow = 0;
		if (A == 1) flow = edmondsKarp(g);
		else if (A == 2) flow = dinic(g);
		if (flow < m + (int) flights.size()) l = m + 1;
		else {
			k = m;
			r = m - 1;
		}
	}
	return k;
}

void instances(int N, int X, int A) {
		string s;
		while (cin >> N >> s) {
			int k = solveFile(s,N,X,A);
			cout << s << " " << k << endl;
		}

}

void times(int N, int X, int A) {
	for (int i = 2; i <= 30; i++) {
		N = i;
		ostringstream convert;
		convert << i;
		string s = "instance_100_" + convert.str() + "_";

		auto t = 0;
		for (int j = 1; j <= 10; j++) {
			ostringstream convert2;
			convert2 << j;
			string s2 = s + convert2.str() + ".air";
			for (int x = 0; x < 10; x++) {
				std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
				solveFile(s2,N,X,A);
				std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
				t += duration;
			}
			//cout << s2 << " " << k << endl;
		}
		t /= 10;
		cout << "N = " << N << " t = " << t << " microseconds" << endl;
	}
}

int main(int argc, char *argv[]) {
	// X defines the capacity of the edges in the different versions
	int X = -1;
	// Number of airports
	int N;
	// Algorithm to be used
	int A;
	parseArgs(argc,argv,N,X,A);

	solve(N,X,A);
	//instances(N,X,A);
	//times(N,X,A);

	return 0;
}
