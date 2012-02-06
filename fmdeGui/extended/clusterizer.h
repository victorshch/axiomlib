#ifndef CLUSTERIZER_H
#define CLUSTERIZER_H

#include <vector>
#include <list>
#include <deque>
#include <boost/tuple/tuple.hpp>

#include "IntInterval.h"

class Clusterizer
{
	std::list<AxiomLib::IntInterval> clusters;
	
	int maxSize_;
	int secondMaxSize_;
	int sumSize_;
	int numClusters_;
	
	int numTraj_;
	
	int sumMaxSize_;
	
	AxiomLib::IntInterval getNextCluster(const std::vector<int> &v, 
										 int minClusterDistance, 
										 int &i);
public:
    Clusterizer();

	void clusterize(const std::vector<int> &v, int minClusterDistance);
	void clear();
	
	int maxSize() const { return maxSize_; }
	int secondMaxSize() const { return secondMaxSize_; }
	int sumSize() const { return sumSize_; }
	int numClusters() const { return numClusters_; }
	
	double aveMaxSize() const;
	double clustersPerTraj() const;
	double goal1() const;
	double goal2() const;
};

#endif // CLUSTERIZER_H

