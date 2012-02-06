#include "OccClustering.h"

using namespace AxiomLib;
using namespace FuzzyMultiDataExt;

OccClustering::OccClustering()
{
	m_minDistance = 2;
	m_maxClusterSize = 0;
	m_minMaxClusterSize = 2;
	
	setOrder(3);
}

void OccClustering::initFromEnvImpl(const Environment &env) {
	env.getIntParamValue(m_minDistance, getParamName("minDistance"));
	env.getIntParamValue(m_maxClusterSize, "maxClusterSize");
	env.getIntParamValue(m_minMaxClusterSize, "minMaxClusterSize");
}

IntInterval OccClustering::getNextCluster(const SatPointSet &satPointSet, 
                                          int multiTSNo, 
                                          int &currentPoint, 
                                          int minClusterDistance) const {
	int currentTSSize = satPointSet.size(multiTSNo);
	while(currentPoint < currentTSSize && !satPointSet.isSat(multiTSNo, currentPoint)) {
		currentPoint++;
	}

	if(currentPoint == currentTSSize) {
		return AxiomLib::IntInterval();
	}
	
	int left = currentPoint;
	int right = currentPoint;
	++currentPoint;
	// Удаление текущей точки от текущей правой границы кластера
	int currentDistance = 1;
	
	
	while(currentDistance < minClusterDistance && currentPoint < currentTSSize) {
		if(satPointSet.isSat(multiTSNo, currentPoint)) {
			right = currentPoint;
			currentDistance = 0;
		}
		currentPoint++;
		currentDistance++;		
	}
	
	return IntInterval(left, right);	
}
