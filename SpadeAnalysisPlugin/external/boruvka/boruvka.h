#ifndef _BORUVKA_
#define _BORUVKA_

#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

//simple edge
typedef struct {
	int v1;
	int v2;
	float weight;
} Edge;

//list of edges in the graph
struct list_edge {
	Edge edge;
	struct list_edge * next;
};

class Boruvka
{
public:

	Boruvka();
	~Boruvka();

	//Read and initialize graph components
	void initGraph(std::vector<std::vector<float> >* _clusterExpressions);
	
	//boruvka algorithm
	void boruvkaMst();

	//Print graph details
	void printData();
	void printData(list_edge * list);

	std::string jsonEdgeList();
	std::string jsonEdgeList(list_edge * list);

	std::vector< std::tuple<int, int, float> > edgeList();

private:
	list_edge *currEdge, *head, *mstTree;
	int number_vertices;
	int number_edges;
	std::vector<int> root;
	std::vector<int> rank;

	//find root for a node
	int findRoot(int i);

	// make root of the node with a smaller rank point to root of the node with a larger rank
	void unionEdge(int i, int j);

	float distance(std::vector<float>* v1, std::vector<float>* v2);
};

#endif //BORUVKA