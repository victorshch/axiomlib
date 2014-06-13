#include "ECStageClustering.h"

#include "ECTypeStage.h"

#include "ClusteringFeatures/MaxValueFeature.h"
#include "ClusteringFeatures/MinValueFeature.h"

#include "ClusteringRealizations/KMeansClusteringModel.h"
#include "ElemCondClustering.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace AxiomLib;
using namespace std;
using namespace AxiomLib::FuzzyMultiDataExt;
using namespace shark;

ECStageClustering::ECStageClustering(FuzzyDataSet* fuzzyDataSet, ECTypeStage* stage0){
	this->stage0 = stage0;
	this->fuzzyDataSet = fuzzyDataSet;
	this->logger = Logger::getInstance();	
	this->parent = stage0->getParent();
}

int ECStageClustering::initFromEnv(const Environment& env){
	set<string> clusteringParams;
	
	if (env.getStringSetParamValue(clusteringParams, "ECClusteringModel")){
		throw AxiomLibException("ECStageClustering::initFromEnv : ECClusteringModel is undefined.");
	}

	vector<string> strs;

	for (auto start = clusteringParams.begin(),
		      end = clusteringParams.end(); start != end; start++)
	{		
		auto clusteringConfiguration = ECClusteringConfiguration::create(fuzzyDataSet, *start);
		clusteringConfigurations.push_back(clusteringConfiguration);
	}
	
	return 0;
}

void ECStageClustering::run(){
	for (auto start = clusteringConfigurations.begin(),
		    end = clusteringConfigurations.end(); start != end; start++)
	{	
		(*start)->run();
	}

	int classCount = fuzzyDataSet->getClassCount();	

	vector<int> dataSetParams;

	fuzzyDataSet->getParamNums(dataSetParams);
	int dimensionsCount = dataSetParams.size();

	int configurationsCount = clusteringConfigurations.size();

	elemConditions.reserve(classCount);

	vector<vector<vector< ElemCondClustering* > > > ecs;
	
	for (int conf = 0; conf < configurationsCount; conf++){
		ecs.push_back(clusteringConfigurations[conf]->getCreatedEC());
	}

	for (int c = 0; c < classCount; c++){
		elemConditions.push_back(vector<vector<vector<ECSelection> > >());
		elemConditions[c].reserve(dimensionsCount);

		for (int dimIndex = 0; dimIndex < dataSetParams.size(); dimIndex++){
			elemConditions[c].push_back(vector<vector<ECSelection> >());
			elemConditions[c][dimIndex].reserve(configurationsCount);

			for (int conf = 0; conf < configurationsCount; conf++){
				elemConditions[c][dimIndex].push_back(vector<ECSelection>());
				for (int n = 0; n < ecs[conf][dimIndex].size(); n++){
					ElemCondPlus ec = ElemCondPlus();
					ec.elemCondition = ecs[conf][dimIndex][n];

					ec.dimension = dataSetParams[dimIndex];

					elemConditions[c][dimIndex][conf].push_back(
						ECSelection(ElemCondPlusStat(ec)));
				}	
			}
		}
	}
}
