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
		throw AxiomLibException("ECClusteringConfiguration::create exception");
	}

	ECClusteringConfiguration* configuration = new ECClusteringConfiguration();
	configuration->fuzzyDataSet = fuzzyDataSet;

	vector<int> dataSetParams;

	fuzzyDataSet->getParamNums(dataSetParams);
	configuration->dimensionsCount = dataSetParams.size();

	boost::trim(strs[0]);
	boost::trim(strs[1]);
	boost::trim(strs[2]);

	configuration->stripLength = atoi(strs[1].c_str());
	configuration->stripsCount = atoi(strs[0].c_str());

	ClusteringModel* clusteringModel = ClusteringModel::create(strs[2]);

	configuration->clusteringModels.push_back(clusteringModel);
	for (int i = 1; i < configuration->dimensionsCount;++i){
		configuration->clusteringModels.push_back(clusteringModel->clone());
	}

	for(int i = 3; i < strs.size(); ++i){
		boost::trim(strs[i]);
		ClusteringFeature* feature = ClusteringFeature::create(strs[i]);
		configuration->clusteringFeatures.push_back(feature);
	}

	configuration->featuresCount = configuration->clusteringFeatures.size();

	return configuration;
}

void ECClusteringConfiguration::run(){
	Logger::comment("Run ECClusteringConfiguration");
	auto dataSetParamNames = fuzzyDataSet->getParamNames();
		
	auto classesCount = fuzzyDataSet->getClassCount();	

	this->clusteringModels.reserve(featuresCount);

	// foreach class
	for (auto i = -1; i < classesCount; ++i){			
		vector<int> numOfTS; 
		int numOfMultiTS; 

		numOfMultiTS = fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Reference, i);

		//foreach dimension
		for (int dim = 1; dim < dimensionsCount + 1; ++dim){

			//foreach trajectory
			for (int j = 0; j < numOfMultiTS; j++){				
				vector<double> row;

				if (i == -1) {
					fuzzyDataSet->getNormalTSFromClass (row, j , dim);
				} else {
					fuzzyDataSet->getTSByIndexFromClass (row, i, j, dim);
				}

				handleTrajectory(row, dim - 1);
			}			
		}
	}
	
	for (int i = 0; i < this->dimensionsCount; i++){
		Logger::comment("Make clustering");
		this->clusteringModels[i]->makeClustering();
		Logger::comment("Stop clustering");
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
