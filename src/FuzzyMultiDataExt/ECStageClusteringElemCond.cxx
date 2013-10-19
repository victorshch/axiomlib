#include "ECStageClusteringElemCond.h"

#include "ClusteringRealizations\SharkClusteringModel.h"

using namespace AxiomLib;
using namespace std;
using namespace AxiomLib::FuzzyMultiDataExt;
using namespace shark;

ECStageClusteringElemCond::ECStageClusteringElemCond(int clusterNumber, int length, const std::list<ClusteringFeature*>& clusteringFeatures, IClusteringModel* model){
	this->clusterNumber = clusterNumber;
	this->length = length;
	this->clusteringFeatures = clusteringFeatures;
	this->clusteringModel = model;
}

signed int ECStageClusteringElemCond::check(const unsigned long k, const std::vector<double>& x) const {
	int featuresCount = clusteringFeatures.size();
	
	vector<double> feature;
	feature.reserve(featuresCount);
	
	for (auto begin = clusteringFeatures.begin(), end = clusteringFeatures.end();
		begin != end; begin++){
			feature.push_back((*begin)->calculate(x, k - length / 2, length));
	}
	
	int clusterNumber = clusteringModel->getClusterNumberToElement(feature);
	
	return clusterNumber == this->clusterNumber;
}