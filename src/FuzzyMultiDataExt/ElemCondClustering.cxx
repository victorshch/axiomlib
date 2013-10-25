#include "ElemCondClustering.h"

#include "ClusteringRealizations\KMeansClusteringModel.h"

using namespace AxiomLib;
using namespace std;
using namespace AxiomLib::FuzzyMultiDataExt;
using namespace shark;

ElemCondClustering::ElemCondClustering(int clusterNumber, int length, const std::list<ClusteringFeature*>& clusteringFeatures, FuzzyMultiDataExt::ClusteringModel* model){
	this->clusterNumber = clusterNumber;
	this->length = length;
	this->clusteringFeatures = clusteringFeatures;
	this->clusteringModel = model;
}

signed int ElemCondClustering::check(const unsigned long k, const std::vector<double>& x) const {
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