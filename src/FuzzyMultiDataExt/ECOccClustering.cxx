#include "../FuzzyMultiDataExtAlgorithm.h"

#include "ECOccClustering.h"

using namespace AxiomLib;
using namespace FuzzyMultiDataExt;

ECOccClustering::ECOccClustering()
{
	
}

std::string ECOccClustering::name() const {
	return "ECOccClustering";
}

int ECOccClustering::stage() const {
	return 1;
}

ECOccClustering* ECOccClustering::clone() const {
	return new ECOccClustering(*this);
}

void ECOccClustering::run(FuzzyMultiDataExtAlgorithm &algorithm, int abType) const {
	for(int dimension = 0; dimension < algorithm.getECSize(abType); dimension++) {
		for(int ecType = 0; ecType < algorithm.getECSize(abType, dimension); ecType++) {
			for(int n = 0; n < algorithm.getECSize(abType, dimension, ecType); n++) {
				if(algorithm.isECSelected(abType, dimension, ecType, n)) {
					bool result = clusterize(algorithm.getEC(abType, dimension, ecType, n), abType, algorithm.getDataSet());
					algorithm.setECSelected(abType, dimension, ecType, n, result);
				}
			}
		}
	}
}
