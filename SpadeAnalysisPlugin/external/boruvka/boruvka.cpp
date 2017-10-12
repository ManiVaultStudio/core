#include "boruvka.h"

#include <math.h>
#include <tuple>

Boruvka::Boruvka()
{
	currEdge = NULL;
	head = NULL;
	mstTree = NULL;

	number_vertices = 0;
	number_edges = 0;
}

Boruvka::~Boruvka(){}

void Boruvka::printData()
{
	printData(mstTree);
}

void Boruvka::printData(list_edge * list)
{
	list_edge *a = list;
	float cost = 0;
	while (a->next != NULL) 
	{
		printf("%02d----%02d  weight: %.2f\n", a->edge.v1, a->edge.v2, a->edge.weight);
		cost += a->edge.weight;
		a = a->next;
	}
	printf("And has a cost of: %.2f\n", cost);
}

std::string Boruvka::jsonEdgeList()
{
	return jsonEdgeList(mstTree);
}

std::string Boruvka::jsonEdgeList(list_edge * list)
{
	list_edge *a = list;

	std::string edgeList = "\"links\":[\n";

	while (a->next != NULL)
	{
		edgeList = edgeList + "{\"source\":" + std::to_string(a->edge.v1) + ", \"target\":" + std::to_string(a->edge.v2) + ", \"value\":" + std::to_string(a->edge.weight) + "}";

		a = a->next;

		if (a->next != NULL) edgeList = edgeList + ",";
		edgeList = edgeList + "\n";
	}

	edgeList = edgeList + "]";

	return edgeList;
}

std::vector< std::tuple<int, int, float> > Boruvka::edgeList()
{
	list_edge *a = mstTree;

	std::vector< std::tuple<int, int, float> > list;
	list.reserve(1000);

	while (a->next != NULL)
	{
		list.push_back(std::make_tuple(a->edge.v1, a->edge.v2, a->edge.weight));

		a = a->next;
	}

	return list;
}

void Boruvka::initGraph(std::vector<std::vector<float> >* _clusterExpressions)
{
	int i;
	head = NULL;
	number_vertices = static_cast<int>(_clusterExpressions->size());
	number_edges = (number_vertices * (number_vertices - 1)) / 2;
	
	root.resize(number_vertices);// = (int*)malloc(number_vertexs * sizeof(int));
	rank.resize(number_vertices);// = (int*)malloc(number_vertexs * sizeof(int));
	head = new list_edge[1];// (list_edge*)malloc(sizeof(list_edge));
	
	currEdge = head;
	for (int v = 0; v < number_vertices; v++)
	{
		for (int e = v+1; e < number_vertices; e++)
		{
			currEdge->edge.v1 = v;
			currEdge->edge.v2 = e;
			currEdge->edge.weight = distance(&(*_clusterExpressions)[v], &(*_clusterExpressions)[e]);

			list_edge *newEdge = new list_edge[1];
			currEdge->next = newEdge;
			currEdge = newEdge;
			currEdge->next = NULL;
		}
	}
	/*
	for (i = 0; i < number_edges; i++)
	{
		list_edge *newEdge = new list_edge[1];// (list_edge*)malloc(sizeof(list_edge));
		fscanf(pFile, "%d", &curr->edge.v1);
		fscanf(pFile, "%d", &curr->edge.v2);
		fscanf(pFile, "%f", &curr->edge.weight);
		newEdge->next = NULL;
		curr->next = newEdge;
		curr = newEdge;
	}*/

	//init root and rank vectors
	for (i = 0; i < number_vertices; i++)
	{
		root[i] = i;
		rank[i] = 1;
	}
	
}
int Boruvka::findRoot(int i)
{
	while (i != root[i]) 
	{
		i = root[i];
	}
	return i;
}

void Boruvka::unionEdge(int i, int j)
{

	if (i == j) 
	{
		return;
	}

	// make smaller root point to larger one
	if (rank[i] < rank[j]) 
	{
		root[i] = j;
		rank[j] += rank[i];
	}
	else 
	{
		root[j] = i;
		rank[i] += rank[j];
	}
}
void Boruvka::boruvkaMst()
{
	list_edge *tree;
	std::vector<Edge> closest = std::vector<Edge>(number_vertices);// (Edge *)malloc(number_vertices * sizeof(Edge));
	mstTree = NULL;
	mstTree = (list_edge*)malloc(sizeof(list_edge));
	tree = mstTree;
	int mstTreeSize = 0;
	int countEdge = 0;
	int iterationCount = 0;
	do {
		for (int i = 0; i < number_vertices; i++)
		{
			closest[i].weight = -1;
		}
		currEdge = head;
		//find closest edge
		while (currEdge->next != NULL)
		{
			int i = findRoot(currEdge->edge.v1);
			int j = findRoot(currEdge->edge.v2);
			if (i == j) 
			{
				currEdge = currEdge->next;
				continue;
			}
			if (closest[i].weight == -1 || currEdge->edge.weight < closest[i].weight)
			{
				closest[i] = currEdge->edge;
			}
			if (closest[j].weight == -1 || currEdge->edge.weight < closest[j].weight)
			{
				closest[j] = currEdge->edge;
			}
			currEdge = currEdge->next;
		}
		countEdge = 0;
		for (int i = 0; i < number_vertices; i++)
		{
			Edge e = closest[i];
			if (e.weight != -1) {
				int u = findRoot(e.v1);
				int v = findRoot(e.v2);
				if (u != v) 
				{
					countEdge++;
					list_edge* newEdge = (list_edge*)malloc(sizeof(list_edge));
					tree->edge = e;
					tree->next = newEdge;
					tree = newEdge;
					mstTreeSize++;
					unionEdge(u, v);
				}
			}
		}
		iterationCount++;
	} while (mstTreeSize < number_vertices - 1);
	tree->next = NULL;
	printf(" Found in: %d iteration(s) \n", iterationCount);
}

float Boruvka::distance(std::vector<float>* v1, std::vector<float>* v2)
{
	float distance = 0.0f;
	for (int i = 0; i < v1->size(); i++)
	{
		float f1 = (*v1)[i];
		float f2 = (*v2)[i];
        float dist = fabs(f1 - f2);

		distance += dist;
	}

	return distance;
}