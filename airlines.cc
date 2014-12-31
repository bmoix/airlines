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

// -----------------------------------

// Funció que crea el graf residual inicial associat a un graf.
IImat graf_to_grafRes(const Graph & graf) {
	IImat aux = IImat (graf.size());
	for (int i = 0; i < (int) graf.size(); ++i) {
		IIvec auxII (graf[i].size());
		for (int j = 0; j < (int) graf[i].size(); ++j) {
			auxII[j] = make_pair(graf[i][j].to, graf[i][j].cap);
		}
		aux[i] = auxII;
	}
	return aux;
}

// Funció que a partir de un graf residual calcula el camí entre l'inici i la fi del graf (si ni ha).
// La funció retorna el valor del bottleneck del camí i modifica el vector "cami" amb aquest si el camí existeix.
// En cas contrari la funció retorna com a bottleneck 0 i el valor del vector "cami" conté dades erronies.
int trobar_cami(const IImat & grafRes, vector<int> & cami) {
	
	// Inicialitzem el bottleneck a 0, si no es troba cap camí aquest serà el valor retornat.
	
	int bottleneck = 0;
	// Eliminem l'informació que contenia cami i l'inicialitzem a -1.
	cami = vector<int> (grafRes.size(), -1);
	// per a qualsevol vèrtex i, 
	// [i] = capacitat màxima que pot passar des de l'inici fins a i.
	vector<int> capacitat = vector<int> (grafRes.size(), 0);

	queue<int> vertexs;
	vertexs.push(S);
	// La capacitat des del inici fins a ell mateix és infinit.
	
	capacitat[S] = 10000;						// ------------S'ha de corregir, solució parcial. ------------
	// Evitem que el camí pugui passar dos cops per el vèrtex inicial.
	cami[S] = -2;

	while (not vertexs.empty()) {
		int vertexIni = vertexs.front();
		vertexs.pop();

		for(int i = 0; i < (int) grafRes[vertexIni].size(); i++) {

			int vertexAct = grafRes[vertexIni][i].first;
			int capacitatTot = grafRes[vertexIni][i].second;
			int capacitatRes = capacitatTot - capacitat[vertexAct];

			if (capacitatRes > 0 and cami[vertexAct] == -1) {
				cami[vertexAct] = vertexIni;
				capacitat[vertexAct] = min(capacitat[vertexIni], capacitatRes);
				if (vertexAct == T) return capacitat[T];
				vertexs.push(vertexAct);
			}
		}
	}
	return bottleneck;

}

// Funció que busca dins el graf residual a quina posició es troba el vèrtex desitjat. En cas de no existir
// el crea i retorna l'index del vector on es troba.
int troba_posicio(IImat & grafRes, int posIni, int posFi) {
	for (int i = 0; i < (int) grafRes[posIni].size(); ++i) {
		if (grafRes[posIni][i].first == posFi) return i;
	}

	// No s'ha trobat node, el crea amb flux inicial 0.
	grafRes[posIni].push_back(make_pair(posFi, 0));
	return grafRes[posIni].size()-1;
}

// Funció que a partir del graf residual, un camí i el bottleneck d'aquest camí actualitza els valors
// dels nodes que formen part del camí del graf residual.
void augment(IImat & grafRes, const vector<int> & cami, int bottleneck) {
	int vertexAct = T;
	while (vertexAct != S){
		int vertexIni = cami[vertexAct];

		// Restem el valor del bottleneck al flux que va en el sentit del camí
		int posMat2 = troba_posicio(grafRes, vertexIni, vertexAct);
		grafRes[vertexIni][posMat2].second -= bottleneck;

		// Sumem el valor del bottleneck al flux que va en sentit contrari al camí
		int posMat1 = troba_posicio(grafRes, vertexAct, vertexIni);
		grafRes[vertexAct][posMat1].second += bottleneck;

		vertexAct = cami[vertexAct];
	}
}

// Funció que a partir del graf, un camí i el bottleneck d'aquest camí actualitza els valors dels
// nodes que formen part del camí del graf.
void actualitza_graf(Graph & graf, const vector<int> & cami, int bottleneck) {
	int vertexAct = T;
	while (vertexAct != S){
		int vertexIni = cami[vertexAct];

		int i = -1;
		while(graf[vertexIni][++i].to != vertexAct);		//Busca la posició del vèrtex en la llista d'adjacències.
		graf[vertexIni][i].flow += bottleneck;

		vertexAct = cami[vertexAct];
	}
}

// Funció que executa el max flow d'un graf.
int max_flow(Graph & graf) {
	IImat grafRes = graf_to_grafRes(graf);
	vector<int> cami;
	int bottleneck;
	int maxflow = 0;
	while ((bottleneck = trobar_cami(grafRes, cami)) > 0) {
		augment(grafRes, cami, bottleneck);
		actualitza_graf(graf, cami, bottleneck);
		maxflow += bottleneck;
	}
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
	int k = 2;
	// Graph that represents the network flow
	Graph g(flights.size()*2+4);
	buildGraph(g,flights,airports,k,X);
	printGraph(g);

	Graph g1 = g;
	// old
	//int maxflow = max_flow(g1);
	// new
	int maxflow = maxFlow(g1);
	cout << endl << "Final graph with value " << maxflow << ":" << endl;
	printGraph(g1);
	
	/*Per provar part maxFlow sola
	Graph g1;
	llegir_entrada(g1);
	int maxflow = maxFlow(g1);
	cout << endl << "Final graph with value " << maxflow << ":" << endl;
	printGraph(g1);*/

	return 0;
}
