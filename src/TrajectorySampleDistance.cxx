#include "TrajectorySampleDistance.h"
#include <iostream>

using namespace AxiomLib;

TrajectorySampleDistance::TrajectorySampleDistance(int nClasses, int length) : m_nClasses(nClasses), m_dist(nClasses),
	m_length(length)
{ }

void AxiomLib::TrajectorySampleDistance::setLength(int newValue)
{
	m_length = newValue;
	
	for(int i = 0; i < m_nClasses; ++i) {
		m_dist[i].resize(length(), -1.0);
	}
}

int TrajectorySampleDistance::numClasses() const
{
	return m_nClasses;
}

int AxiomLib::TrajectorySampleDistance::length() const
{
	return m_length;
}

void AxiomLib::TrajectorySampleDistance::setDistance(int classNo, int endPoint, double distance)
{
        //todo checks in debug mode
	m_dist[classNo][endPoint] = distance;
}

void TrajectorySampleDistance::setDistancesForClass(int classNo, const std::vector<double> &distances)
{
	m_dist[classNo] = distances;
}

//void AxiomLib::TrajectorySampleDistance::setDistancesForClass(int classNo, const std::vector<double> &labels)
//{
//	if(length() != labels.size()) {
//		throw AxiomLibException("TrajectorySampleDistance::setDistancesForClass : invalid labels length");
//	}
	
//	m_dist[classNo] = labels;
//}

double AxiomLib::TrajectorySampleDistance::distance(int classNo, int point) const
{
	//todo checks in debug mode
	return m_dist[classNo][point];
}
