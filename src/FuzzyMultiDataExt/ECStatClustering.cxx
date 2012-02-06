#include <boost/tuple/tuple.hpp>

#include "../FuzzyMultiDataExtAlgorithm.h"

#include "ECStatClustering.h"

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

ECStatClustering::ECStatClustering()
{
	this->setOrder(1);
}

ECStatClustering* ECStatClustering::clone() const {
	return new ECStatClustering(*this);
}

std::string ECStatClustering::name() const {
	return "ECStatClustering";
}

int ECStatClustering::stage() const {
	return 1;
}

void ECStatClustering::run(FuzzyMultiDataExtAlgorithm &algorithm, int abType) const {
	std::vector<StatVector> statistics;
	std::vector<int> indices;
	std::vector<bool> result;
	
	for(int dimension = 0; dimension < algorithm.getECSize(abType); ++dimension) {
		for(int ecType = 0; ecType < algorithm.getECSize(abType, dimension); ++ecType) {
			int numECs = algorithm.getECSize(abType, dimension, ecType);
			
			statistics.clear();
			statistics.reserve(numECs);
			
			indices.clear();
			indices.reserve(numECs);
			
			for(int n = 0; n < numECs; ++n) {
				if(algorithm.isECSelected(abType, dimension, ecType, n)) {
					const ElemCondPlusStat& currentEC = algorithm.getEC(abType, dimension, ecType, n);
					StatVector current = StatVector(
								currentEC.goal,
								currentEC.statAbnorm,
								currentEC.statNormal,
								currentEC.statOccurence
								);
					statistics.push_back(
								current
								);
					indices.push_back(n);
				}
			}
			
			this->clusterize(statistics, result);
			
			for(int i = 0; i < indices.size(); i++) {
				algorithm.setECSelected(abType, dimension, ecType, indices[i], result[i]);
			}
		}
	}
}
