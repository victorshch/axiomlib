#include "../FuzzyMultiDataExtAlgorithm.h"

#include "AXOccClustering.h"

using namespace AxiomLib;
using namespace FuzzyMultiDataExt;

AXOccClustering::AXOccClustering()
{
}

std::string AXOccClustering::name() const {
	return "AXOccClustering";
}

int AXOccClustering::stage() const {
	return 2;
}

AXOccClustering* AXOccClustering::clone() const {
	return new AXOccClustering(*this);
}


void AXOccClustering::run(FuzzyMultiDataExtAlgorithm &algorithm, int abType) const {
	for(int n = 0; n < algorithm.getAXSize(abType); n++) {
		if(algorithm.isAXSelected(abType, n)) {
			bool result = clusterize(algorithm.getAX(abType, n), abType, algorithm.getDataSet());
			algorithm.setAXSelected(abType, n, result);
		}
	}
}
