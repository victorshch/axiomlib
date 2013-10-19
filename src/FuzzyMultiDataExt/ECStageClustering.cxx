#include "ECStageClustering.h"
#include "Clusteringfeatures/MaxValuefeature.h"

#include "ClusteringRealizations\SharkClusteringModel.h"
#include "ECStageClusteringElemCond.h"

using namespace AxiomLib;
using namespace std;
using namespace AxiomLib::FuzzyMultiDataExt;
using namespace shark;

ECStageClustering::ECStageClustering(FuzzyDataSet* fuzzyDataSet, ECTypeStage* stage0){
	this->stage0 = stage0;
	this->fuzzyDataSet = fuzzyDataSet;
	this->logger = Logger::getInstance();	
}

int ECStageClustering::initFromEnv(const Environment& env){
	set<string> clusteringParams;
	if (!env.getStringSetParamValue(clusteringParams, "ECClustering")){
		throw AxiomLibException("ECStageClustering::initFromEnv : ECClustering is undefined.");
	}

	for (auto start = clusteringParams.begin(), 
		 end = clusteringParams.end(); start != end; start++){
			 // Parse params and create Clustering features using class ClusteringfeatureFactory
	}

	this->clusteringFeatures.push_back(&MaxValueFeature());

	this->featuresCount = clusteringFeatures.size();
}

void ECStageClustering::handleTrajectory(const vector<double>& trajectory, int dimension) {	
	int length = trajectory.size();

	for (int i = 0; i < stripsCount; i++){
		vector<double> feature;
		feature.reserve(featuresCount);
		// todo: use reserve

		int position = rand() % (length - stripLength);
		for (auto begin = clusteringFeatures.begin(), end = clusteringFeatures.end();
			begin != end; begin++){
				feature.push_back((*begin)->calculate(trajectory, position, stripLength));
		}

		this->clusteringModels[dimension]->addElement(feature);
	}
}

void ECStageClustering::run(){
	vector<int> dataSetParams;

	this->fuzzyDataSet->getParamNums(dataSetParams);
	dimensions = dataSetParams.size();

	auto dataSetParamNames = fuzzyDataSet->getParamNames();
		
	auto classesCount = fuzzyDataSet->getClassCount();	

	resultFeatures.resize(dimensions);

	this->clusteringModels = new (IClusteringModel*[featuresCount]);

	for(int i = 0; i < this->dimensions; i++){
		this->clusteringModels[i] = new SharkClusteringModel();
	}

	// foreach class
	for (auto i = 0; i < classesCount; ++i){			
		vector<int> numOfTS; 
		int numOfMultiTS; 
		fuzzyDataSet->getNormalClassSize(numOfMultiTS, numOfTS);
		
		//foreach dimension
		for (int k = 0; k < dimensions; ++k){

			//foreach trajectory
			for (int j = 0; j < numOfMultiTS; j++){				
				vector<double> row;
				fuzzyDataSet->getNormalTSFromClass (row, j , k);
				handleTrajectory(row, k);
			}			
		}
	}

	auto feautureVectorSize = this->clusteringFeatures.size();

	for (int i = 0; i < this->dimensions; i++){
		this->clusteringModels[i]->makeClustering();
	}

	for (int j = 0; j < dimensions; j++){
		for (int i = 0; i < this->k; i++){
			this->elemCond.push_back(new ECStageClusteringElemCond(i, stripLength, clusteringFeatures, this->clusteringModels[i]));
		}
	}
}

