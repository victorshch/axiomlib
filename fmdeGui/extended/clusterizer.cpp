#include <numeric>

#include "clusterizer.h"
#include <QtDebug>

Clusterizer::Clusterizer() : 
		maxSize_(0), 
		secondMaxSize_(0), 
		sumSize_(0),
		sumMaxSize_(0),
		numClusters_(0),
		numTraj_(0) {
	
}

void Clusterizer::clusterize(const std::vector<int> &v, int minClusterDistance) {
	qDebug()<<"entering Clusterizer::clusterize()";
	++numTraj_;
	int i = 0;
	while(true) {
		qDebug()<<"getting next cluster";
		AxiomLib::IntInterval cluster = getNextCluster(v, minClusterDistance, i);
		qDebug()<<"got next cluster";
		if(cluster.left() < 0) {
			break;
		}
		clusters.push_back(cluster);
		int size = cluster.length();
		if(size > maxSize_) {
			secondMaxSize_ = maxSize_;
			maxSize_ = size;
		} else if(size > secondMaxSize_) {
			secondMaxSize_ = size;
		}
		sumSize_ += size;
		numClusters_++;
	}
	sumMaxSize_ += maxSize_;
}

void Clusterizer::clear() {
	*this = Clusterizer();
}

AxiomLib::IntInterval Clusterizer::getNextCluster(const std::vector<int> &v, 
												  int minClusterDistance, 
												  int &i) {
	//qDebug()<<"entering Clusterizer::getNextCluster(). i = "<<i<<" v.size() == "<<v.size();
	while(i < (int)v.size() && v[i] == 0) {
		//qDebug()<<"loop: i == "<<i<<" v[i] == "<<v[i];
		i++;
	}

	qDebug()<<"i == "<<i;

	if(i == (int) v.size()) {
		return AxiomLib::IntInterval();
	}
	
	int left = i;
	int right = i;
	++i;
	int currentDistance = 1;
	
	while(currentDistance < minClusterDistance && i <(int) v.size()) {
		if(v[i] != 0) {
			right = i;
			currentDistance = 0;
		}
		i++;
		currentDistance++;		
	}
	//qDebug()<<"exiting Clusterizer::getNextCluster()";
	return AxiomLib::IntInterval(left, right);
}

double Clusterizer::aveMaxSize() const {
	return (double) sumMaxSize_ / (double) numTraj_;
}

double Clusterizer::clustersPerTraj() const {
	return (double) this->numClusters() / (double) this->numTraj_;
}

double Clusterizer::goal1() const {
	return (double) this->maxSize() / (double) this->sumSize();
}

double Clusterizer::goal2() const {
	return (double) this->maxSize() / (double) this->secondMaxSize();
}
