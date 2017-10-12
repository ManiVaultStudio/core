#ifndef _HCLUSTER_H_
#define _HCLUSTER_H_

#include <math.h>

#include<map>
#include<list>
#include<vector>

struct cPoint_t
{
  int index;               // ordered index of this point
  std::vector<double> expression;    // position of the point
  double data;             // measurement
  double min_dist_to_set;  // R_index
  int parent_index;        // parent index
  std::list<int> children;      // children of this node

  int originalIndex;		   // index in the file of the original data
  int fileIndex;           // index of the file this point belongs to
};

void hcluster_points(std::list<cPoint_t*> &input_points, std::map<int, cPoint_t*> &output_points, bool print = false);
void compute_n_clusters(int cluster_count, std::map<int, cPoint_t*> &output_points,
	std::vector<std::list<cPoint_t*> > &clusters);
#endif
