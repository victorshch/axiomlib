#include <boost/tuple/tuple.hpp>

#include "../FuzzyMultiDataExtAlgorithm.h"

#include "AXStatClustering.h"

using namespace AxiomLib;
using namespace FuzzyMultiDataExt;

AXStatClustering::AXStatClustering()
{
	setOrder(1);
}

std::string AXStatClustering::name() const {
	return "AXStatClustering";
}

int AXStatClustering::stage() const {
	return 2;
}

AXStatClustering* AXStatClustering::clone() const {
	return new AXStatClustering(*this);
}

void AXStatClustering::run(FuzzyMultiDataExtAlgorithm &algorithm, int abType) const {
	std::vector<StatVector> statistics;
	std::vector<int> indices;
	std::vector<bool> result;
	
	int axiomCount = algorithm.getAXSize(abType);
	
	statistics.clear();
	statistics.reserve(axiomCount);
	
	indices.clear();
	indices.reserve(axiomCount);
	
	for(int n = 0; n < axiomCount; n++) {
		if(algorithm.isAXSelected(abType, n)) {
			const AxiomExprPlus& currentAx = algorithm.getAX(abType, n);
			statistics.push_back(
						StatVector(
							currentAx.goal,
							currentAx.statAbnorm,
							currentAx.statNormal,
							currentAx.statOccurence,
							currentAx.statVariation
							)
						);
			
			indices.push_back(n);
		}
	}
	
	this->clusterize(statistics, result);
	
	for(int i = 0; i < indices.size(); i++) {
		algorithm.setAXSelected(abType, indices[i], result[i]);
	}
	

}
