#include <iostream>
#include <vector>
#include <queue>
#include <stack>

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

void parseArgs(int argc, char *argv[], int& N, int& X) {
	if (argc != 3) {
		cout << "Usage: airlines <airports> <version>" << endl;
		exit(1);
	}
	std::string arg1 = argv[1];
	std::string arg2 = argv[2];
	N = atoi(arg1.c_str());
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

void buildGraph(Graph& g, vector<Flight>& flights, MI& airports, int k, int X) {
	g[S].push_back(Edge(s,k,0));
	g[t].push_back(Edge(T,k,0));
	g[s].push_back(Edge(t,k,0));
	int i = 4;
	for (Flight f : flights) {
		g[i].push_back(Edge(i+1,X-1,0));
		g[i].push_back(Edge(T,1,0));
		g[s].push_back(Edge(i,X,0));
		g[i+1].push_back(Edge(t,1,0));
		g[S].push_back(Edge(i+1,1,0));
		for (auto a : airports[f.dest]) {
			if (flights[a].dept - f.arr >= 15) {
				g[i+1].push_back(Edge(4+2*a,X,0));
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

void updateK(Graph& g, int k) {
	g[S][0].cap = k;
	g[t][0].cap = k;
	g[s][0].cap = k;
}

// -----------------------------------

void llegir_entrada(Graph &graf) {
	int numVer;						// Número de vertex que té el graf
	cin >> numVer;
	graf = Graph (numVer);			// Llista d'adjacencia que representa el graf

	int ver1, ver2, capacitat;
	while (cin >> ver1 >> ver2 >> capacitat) {
		graf[ver1].push_back(Edge(ver2,capacitat,0));
	}
}

void escriure_cami(const VI & cami) {
	stack<int> aux;
	int ini = T;
	while (ini != S) {
		aux.push(ini);
		ini = cami[ini];
	}
	aux.push(S);
	while (not aux.empty()) {
		cout << aux.top() << " ";
		aux.pop();
	}
	cout << endl;
}

void buildResidual(const Graph & g, Graph & r) {
	for (int i = 0; i < (int)g.size(); i++) {
		for (int j = 0; j < (int)g[i].size(); j++) {
			r[i].push_back((Edge(g[i][j].to,g[i][j].cap, g[i][j].cap)));
		}
	}
}

void residualToGraph(const Graph & r, Graph & g) {
	for (int i = 0; i < (int) r.size(); i++) {
		for (int j = 0; j < (int) r[i].size(); j++) {
			if (r[i][j].cap != -1) g[i].push_back((Edge(r[i][j].to,r[i][j].cap, r[i][j].cap - r[i][j].flow)));
		}
	}
}

int findPath(const Graph& r, VI& path) {
	int bottleneck = 0;
	path = VI(r.size(),-1);
	VI cap(r.size(),0);
	queue<int> vertexs;
	vertexs.push(S);
	cap[S] = 10000;			// Partial solution, is important to fix it.
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
		// augment part
		int p1 = findPosition(r,vAct,vIni);
		r[vAct][p1].flow += bottleneck;
		int p2 = findPosition(r,vIni,vAct);
		r[vIni][p2].flow -= bottleneck;

		vAct = path[vAct];
	}
}

int maxFlow(Graph & g) {
	Graph r(g.size());
	buildResidual(g,r);
	VI path;
	int bottleneck, maxflow=0;
	while((bottleneck = findPath(r,path)) > 0) {
		augment(r,path,bottleneck);
		maxflow += bottleneck;
	}
	Graph g1(g.size());
	residualToGraph(r, g1);
	g = g1;
	return maxflow;
}

int main(int argc, char *argv[]) {
	// X defines the capacity of the edges in the different versions
	int X = -1;
	// Number of airports
	int N;
	parseArgs(argc,argv,N,X);

	// All the flights with it's info
	vector<Flight> flights;
	// Flights of each airport
	vector<vector<int>> airports(N);
	readFlights(flights,airports,X);

	// number of pilots to serve all flights
	int k=0;
	// Graph that represents the network flow
	Graph g(flights.size()*2+4);
	buildGraph(g,flights,airports,k,X);
	
	// Binary search to find the minimum k
	int l = 0, r = flights.size();
	while (l <= r) {
		int m = (l+r)/2;
		updateK(g,m);
		Graph g1 = g;
		int flow = maxFlow(g1);
		if (flow < m + flights.size()) l = m + 1;
		else {
			k = m;
			r = m - 1;
		}
	}
	cout << "k " << k << endl;

	return 0;
}
