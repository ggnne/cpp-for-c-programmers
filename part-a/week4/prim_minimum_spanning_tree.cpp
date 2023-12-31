// Prim Minimum spanning tree algorithm implementation

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <limits>
#include <climits>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <utility>
#include <fstream>

// ===================================================================== 
// Definitions of the smallest components: Edge as a struct with a cost
// double and an int to to identify the next Node, while Node is another 
// struct with an int n as id and a vector to store its edges 
// =====================================================================

struct Edge{
  int to;
  double cost;
};

struct Node{
    int n;
    std::map<int, Edge> edges;
};

// ===================================================================== 
// Graph Class
// ===================================================================== 

class Graph{
  
  public:
    Graph();
    Graph(int numVertices);
    Graph(std::fstream &input_file);
    void addVertex(); // add an additional vertex to the graph.
    std::vector<int> getVertices(); // return a vector of vertices in the graph
    bool adjacent(int x, int y); // tests whether there is an edge from node x to node y.
    std::vector<int> neighbors(int x); // lists all nodes y such that there is an edge from x to y.
    void addEdge(int x, int y, double c); // adds the edge from x to y, if it is not there.
    void removeEdge(int x, int y); // removes the edge from x to y, if it is there.
    std::map<int, Edge> getEdges(int x);
    double getEdgeCost(int x, int y); // returns the value associated to the edge (x,y).
    void setEdgeCost(int x, int y, double c); // sets the value associated to the edge (x,y) to v.
    int V() const; // returns the number of vertices in the graph
    int E() const; // returns the number of edges in the graph
    void printGraph(); // print the graphs as an adjacent list
    void printAdjMatrix(); // print graph as a adjacency matrix
    
  private:
    int numV; // number of Vertices
    int numE; // number of Edges
    std::vector<Node> adjList; // adjucency list representing the Graph
    
};

Graph::Graph()
{
  numV = 0;
  numE = 0;
  adjList.clear();
}

Graph::Graph(int numVertices)
{
  numV = 0;
  numE = 0;
  adjList.clear();
  
  for(int i = 0; i < numVertices; ++i){
    addVertex();
  }
}

Graph::Graph(std::fstream &input_file){

    numV = 0;
    numE = 0;
    adjList.clear();

    int numVertices, x, y, cost;
    input_file >> numVertices;

    for (int i = 0; i < numVertices; ++i){
        addVertex();
    }

    while (input_file >> x >> y >> cost){
        addEdge(x, y, cost);
    }
}

void Graph::addVertex(){
  Node newNode;
  newNode.n = numV;
  adjList.push_back(newNode);
  numV++;
}

std::vector<int> Graph::getVertices(){
    std::vector<int> vec;
    for(auto it = adjList.begin(); it != adjList.end(); ++it){
        vec.push_back((*it).n);
    }
    return vec;
}

bool Graph::adjacent(int x, int y){
    return adjList[x].edges.count(y);
}

std::vector<int> Graph::neighbors(int x){
    std::vector<int> vec;
    for(auto it = adjList[x].edges.begin(); it != adjList[x].edges.end(); ++it){
        vec.push_back(it->first);
    }
    return vec;
}

void Graph::addEdge(int x, int y, double c){
    
    if (!adjList[x].edges.count(y)){
        Edge xy = {y, c}, yx = {x, c};
        adjList[x].edges.insert({y, xy});
        adjList[y].edges.insert({x, yx});
        numE+=2;        
    }
}

void Graph::removeEdge(int x, int y){
    
    if (adjList[x].edges.count(y)){
        adjList[x].edges.erase(y);
        adjList[y].edges.erase(x);
        numE-=2;
    }
}

std::map<int, Edge> Graph::getEdges(int x){
    return adjList[x].edges;
}

double Graph::getEdgeCost(int x, int y){
    
    if (adjList[x].edges.count(y)){
        return adjList[x].edges[y].cost;
    }
    return static_cast<double>(INT_MAX);
}

void Graph::setEdgeCost(int x, int y, double c){

    if (adjList[x].edges.count(y)){
        adjList[x].edges[y].cost = c;
        adjList[y].edges[x].cost = c;
    }
 
}

int Graph::V() const{
    return numV;
}

int Graph::E() const{
    return numE;
}

void Graph::printGraph(){
  std::cout << "From: [To (Cost), ... ]" << std::endl;
  for(int i = 0; i < numV; ++i){
      std::cout << adjList[i].n << ": ";
      for (auto it = adjList[i].edges.begin(); it != adjList[i].edges.end(); ++it){
          std::cout << it->first << " (" << std::setprecision(2) << (it->second).cost << "), ";
      }
      std::cout << std::endl;
  }
}

void Graph::printAdjMatrix(){
    
    std::cout << ' ' << ' ';
    for(int i = 0; i < numV; ++i){
        std::cout << ' ' << std::setw(4) << i;
    }
    std::cout << std::endl;

  for(int i = 0; i < numV; ++i){
      std::cout << std::setw(2) << i ;
      for(int j = 0; j < numV; ++j){
         std::cout << ' ' << std::setw(4);
        if (adjacent(i, j)){
            std::cout << std::setprecision(2) << getEdgeCost(i, j);
        } else {
            std::cout << 'x';
        }  
      }
      std::cout << std::endl;
    }
}

// ===================================================================== 
// NodeInfo class to use with priority_queue in Shortestpath
// ===================================================================== 

class NodeInfo{
    public:
        int from;
        int to;
        double dist;
        bool operator<(const NodeInfo& n) const{ // so can use it with priority_queue
            return !(this->dist < n.dist);
        }
};

// ===================================================================== 
// ShortestPath Class (using Dijkstra)
// ===================================================================== 

class ShortestPath{
    public:
        ShortestPath();
        ShortestPath(Graph *graph);
        void addGraph(Graph *graph);
        void runShortestPath(int source);
        double avgPathLenght() const;
        double avgPathDist() const;
        std::list<int> path(int from, int to);
        int pathSize(int from, int to);
        double minDist(int from, int to);
    
    private:
        Graph* g;
        int last_run;
        std::map<int, std::list<int>> paths;
        std::map<int, int> path_sizes;
        std::map<int, double> min_distances;
        double avg_path_length;
        double avg_path_dist;
};

ShortestPath::ShortestPath() {}

ShortestPath::ShortestPath(Graph *graph){
    g = graph;
}


void ShortestPath::addGraph(Graph *graph){
    g = graph;
}


double ShortestPath::avgPathLenght() const {
    return avg_path_length;
}


double ShortestPath::avgPathDist() const {
    return avg_path_dist;
}


std::list<int> ShortestPath::path(int from, int to){
    
    if (from != last_run){
        runShortestPath(from);
    }
    
    if (paths.count(to)){
        return paths[to];
    }
    
    std::list<int> el;
    return el;
}


int ShortestPath::pathSize(int from, int to){
    
    if (from != last_run){
        runShortestPath(from);
    }
    
    if (path_sizes.count(to)) {
        return path_sizes[to];    
    }
    
    return INT_MAX;       
}


double ShortestPath::minDist(int from, int to){
    
    if (from != last_run){
        runShortestPath(from);
    }
    
    if (min_distances.count(to)){
        return min_distances[to];
    }
    
    return static_cast<double>(INT_MAX);
}


void ShortestPath::runShortestPath(int source){
    
    last_run = source;
    paths.clear();
    path_sizes.clear();
    min_distances.clear();
    avg_path_length = 0.0;
    avg_path_dist = 0.0;
    
    std::priority_queue<NodeInfo> pq;
    std::set<int> closed;
    std::map<int, std::pair<int, double>> pathCost;
    
    int curr = source;
    pathCost[curr] = std::make_pair(curr, 0.0);
    auto edges = (*g).getEdges(curr);
    for(auto it = edges.begin(); it != edges.end(); ++it){
        NodeInfo n = {curr, (it->second).to, (it->second).cost + pathCost[curr].second};
        pq.push(n);
    }
    closed.insert(curr);
    
    while (!pq.empty()){
        
        NodeInfo top = pq.top();
        pq.pop();
        if (!pathCost.count(top.to) || (top.dist < pathCost[top.to].second)){
            pathCost[top.to] = std::make_pair(top.from, top.dist);
        }
        
        curr = top.to;
        edges = (*g).getEdges(curr);
        for(auto it = edges.begin(); it != edges.end(); ++it){
            if (!closed.count((it->second).to)){
                NodeInfo n = {curr, (it->second).to, (it->second).cost + pathCost[curr].second};
                pq.push(n);
            }
        }
        closed.insert(curr);
        
    }
    
    int cursor;
    int path_counter = 0;
    for (auto it = pathCost.begin(); it != pathCost.end(); ++it){
                
        cursor = it->first;
        if(cursor == source) continue;
        
        double dist = (it->second).second;
        
        min_distances.insert({it->first, dist});
        
        std::list<int> path;
        
        path.push_front(cursor);
        while ((pathCost[cursor].first) != source){
            cursor = pathCost[cursor].first;
            path.push_front(cursor);
            path_counter++;
        }
        path.push_front(source);
        paths.insert({it->first, path});
        
        path_sizes.insert({it->first, path.size()-1});
        
        avg_path_length += ((path.size()-1) - avg_path_length) / ++path_counter;
        avg_path_dist += (dist - avg_path_dist) / path_counter;
        
    }
        
}

// ===================================================================== 
// Montecarlo simulaiton
// ===================================================================== 

class Montecarlo{
    public:
        Montecarlo();
        void run(int vertices, double density, double min_cost, double max_cost); 
};

Montecarlo::Montecarlo() {
    srand(time(NULL));    
}


void Montecarlo::run(int vertices, double density, double min_cost, double max_cost){
    
    for (int i = 0; i < vertices*5 + 2; ++i){
        std::cout << "=";
    }
    std::cout << std::endl;
    
    std::cout << "Running simulation..." << std::endl;
    std::cout << std::endl;
    
    Graph G(vertices);
    
    double p, cost;
    for(int i = 0; i < vertices-1; ++i){
        for(int j = i + 1; j < vertices; ++j){
            p = static_cast<double>(rand()) / RAND_MAX;
            if (p <= density){
                cost = (p/density) * (max_cost - min_cost) + min_cost;
                G.addEdge(i, j, cost);
            }   
        }    
    }
    
    std::cout << "Vertices: " << vertices << std::endl;
    std::cout << "Density: " << std::setprecision(2) << density << std::endl;
    std::cout << "Min-Max cost: " << static_cast<int>(min_cost) << " - " << static_cast<int>(max_cost) << std::endl;
    
    std::cout << std::endl;
    std::cout << "Results:" << std::endl;
    double actual_density = static_cast<double>(G.E()) / (G.V()*(G.V()-1));
    std::cout << "Achieved density: " << std::setw(5) << std::setprecision(4) << actual_density << std::endl;
    
    ShortestPath DSP(&G);
    DSP.runShortestPath(0);
    
    std::cout << "Average (Shortest Dijkstra)Path lenght: " << std::setprecision(2) << DSP.avgPathLenght() << std::endl;
    std::cout << "Average (Shortest Dijkstra)Path distance: " << std::setprecision(2) << DSP.avgPathDist() << std::endl;
    std::cout << std::endl;
    
    std::cout << "Adjacency Matrix" << std::endl;
    G.printAdjMatrix();
    
    std::cout << std::endl;
    
    std::cout << "Shortest Paths from 0:" << std::endl;
    
    for (int i = 1; i < G.V(); ++i){
        
        std::cout << "To " << std::setw(2) << i << " (" << std::setw(7) << std::setprecision(2) << DSP.minDist(0, i) << "): ";
        auto path = DSP.path(0, i);
        for (auto it = path.begin(); it != path.end(); ++it){
            std::cout << *it;
            if (it != --path.end()){
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;
    }
    
    for (int i = 0; i < vertices*5 + 2; ++i){
        std::cout << "=";
    }
    std::cout << std::endl;
}

// =====================================================================
// Prim algorithm class
// =====================================================================

class Prim{
public:
    Prim();
    Prim(Graph *graph);
    void addGraph(Graph *graph);
    void run(int source);
    Graph getMST() const;
    double getMSTCost() const;

private:
    Graph *g;
    Graph MST;
    double MSTCost;
};

Prim::Prim(){
    MSTCost = 0.0;
}

Prim::Prim(Graph *graph)
{
    MSTCost = 0.0;
    g = graph;
    Graph newMST((*graph).V());
    MST = newMST;
}

void Prim::addGraph(Graph *graph){
    MSTCost = 0.0;
    g = graph;
    Graph newMST((*graph).V());
    MST = newMST;
}

void Prim::run(int source){

    std::priority_queue<NodeInfo> pq;
    std::set<int> visited;

    int curr = source;
    auto edges = (*g).getEdges(curr);
    for (auto it = edges.begin(); it != edges.end(); ++it)
    {
        NodeInfo n = {curr, (it->second).to, (it->second).cost};
        pq.push(n);
    }
    visited.insert(curr);

    while (visited.size() != (*g).V()){

        NodeInfo top = pq.top();
        pq.pop();
        if (!visited.count(top.to)){
            MST.addEdge(top.from, top.to, top.dist);
            MSTCost += top.dist;
        }

        curr = top.to;
        edges = (*g).getEdges(curr);
        for (auto it = edges.begin(); it != edges.end(); ++it)
        {
            if (!visited.count((it->second).to))
            {
                NodeInfo n = {curr, (it->second).to, (it->second).cost};
                pq.push(n);
            }
        }
        visited.insert(curr);

        if (pq.empty())
            break;

    }
}

Graph Prim::getMST() const{
    return MST;
}

double Prim::getMSTCost() const{
    return MSTCost;
}

int main() {

    std::fstream fin("sample_data.txt", std::fstream::in);
    Graph G(fin);

    std::cout << "Initial Graph" << std::endl;
    G.printGraph();
    std::cout << std::endl;

    Prim prim(&G);
    prim.run(0);

    Graph MST = prim.getMST();
    double MSTCost = prim.getMSTCost();

    std::cout << "Minimum Spanning Tree" << std::endl;
    MST.printGraph();
    std::cout << std::endl;
    std::cout << "Minimum Spanning Tree Cost: " << MSTCost << std::endl;

    fin.close();

    return 0;
}
