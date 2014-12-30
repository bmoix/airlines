#include <iostream>
#include <vector>

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

typedef vector<vector<Edge>> Graph;

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

void readFlights(vector<Flight>& flights, vector<vector<int>>& airports, int& X) {
	int o, d, h1, h2, i=0;
	while (cin >> o >> d >> h1 >> h2) {
		flights.push_back(Flight(o,d,h1,h2));
		airports[o].push_back(i++);
	}
	if (X == -1) X = flights.size();
	
}

void buildGraph(Graph& g, vector<Flight>& flights, vector<vector<int>>& airports, int k, int X) {
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
	int k = 0;
	// Graph that represents the network flow
	Graph g(flights.size()*2+4);
	buildGraph(g,flights,airports,k,X);
	printGraph(g);

	// solve maxflow
	return 0;
}
