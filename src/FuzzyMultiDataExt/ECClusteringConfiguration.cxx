#include "ECClusteringConfiguration.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;
using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

ECClusteringConfiguration* ECClusteringConfiguration::create(const FuzzyDataSet* fuzzyDataSet, const std::string& s)
{
	vector<string> strs;
	boost::split(strs, s, boost::is_any_of(","));

	if (strs.size() < 2) {
		throw AxiomLibException();
	}

	ECClusteringConfiguration* configuration = new ECClusteringConfiguration();
	configuration->fuzzyDataSet = fuzzyDataSet;

	vector<int> dataSetParams;

	fuzzyDataSet->getParamNums(dataSetParams);
	configuration->dimensionsCount = dataSetParams.size();

	boost::trim(strs[0]);

	ClusteringModel* clusteringModel = ClusteringModel::create(strs[0]);

	configuration->clusteringModels.push_back(clusteringModel);
	for (int i = 1; i < configuration->dimensionsCount;++i){
		configuration->clusteringModels.push_back(clusteringModel->clone());
	}

	for(int i = 1; i < strs.size(); ++i){
		boost::trim(strs[1]);
		ClusteringFeature* feature = ClusteringFeature::create(strs[i]);
		configuration->clusteringFeatures.push_back(feature);
	}

	configuration->featuresCount = strs.size() - 1;

	return configuration;
}

void ECClusteringConfiguration::run(){
	auto dataSetParamNames = fuzzyDataSet->getParamNames();
		
	auto classesCount = fuzzyDataSet->getClassCount();	

	this->clusteringModels.reserve(featuresCount);

	// foreach class
	for (auto i = -1; i < classesCount; ++i){			
		vector<int> numOfTS; 
		int numOfMultiTS; 

		numOfMultiTS = fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Reference, i);

		//foreach dimension
		for (int dim = 0; dim < dimensionsCount; ++dim){

			//foreach trajectory
			for (int j = 0; j < numOfMultiTS; j++){				
				vector<double> row;

				if (i == -1) {
					fuzzyDataSet->getNormalTSFromClass (row, j , dim);
				} else {
					fuzzyDataSet->getTSByIndexFromClass (row, i, j, dim);
				}

				handleTrajectory(row, dim);
			}			
		}
	}

	for (int i = 0; i < this->dimensionsCount; i++){
		this->clusteringModels[i]->makeClustering();
	}

	for (int i = 0; i < this->clusteringModels.size(); i++){
		elemCond.push_back(vector<ElemCondClustering*>());
		for (int j = 0; j < this->clusteringModels[i]->getClustersCount(); j++){
			elemCond[i].push_back(new ElemCondClustering(
				j, 
				stripLength, 
				clusteringFeatures, 
				this->clusteringModels[i])
			);
		}
	} 
}

void ECClusteringConfiguration::handleTrajectory(const vector<double>& trajectory, int dimension) {	
	int length = trajectory.size();

	for (int i = 0; i < stripsCount; i++){
		vector<double> feature;
		feature.reserve(featuresCount);

		int position = rand() % (length - stripLength);
		for (auto begin = clusteringFeatures.begin(), end = clusteringFeatures.end();
			begin != end; begin++){
				feature.push_back((*begin)->calculate(trajectory, position, stripLength));
		}

		this->clusteringModels[dimension]->addElement(feature);
	}
}
