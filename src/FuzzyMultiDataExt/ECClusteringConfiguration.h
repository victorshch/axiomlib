#pragma once

#ifndef FUZZYMULTIDATAEXT_EC_CLUSTERING_CONFIGURATION_H
#define FUZZYMULTIDATAEXT_EC_CLUSTERING_CONFIGURATION_H

#include "../Environment.h"
#include "../FuzzyDataSet.h"
#include "Common.h"
#include "ECStage.h"
#include "ForwardDeclarations.h"
#include "ClusteringFeatures/ClusteringFeature.h"
#include "ClusteringRealizations/ClusteringModel.h"
#include "ElemCondClustering.h"

using namespace std;

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class ECClusteringConfiguration {				
	public:						
		static ECClusteringConfiguration* create(const FuzzyDataSet* fuzzyDataSet, const std::string& s);				
		
		
		void run();


		std::vector<std::vector<ElemCondClustering*> > getCreatedEC() { return elemCond; }
	private:			
		void handleTrajectory(const std::vector<double>& trajectory, int dimension);			

		const FuzzyDataSet* fuzzyDataSet;
			
		// dimensions count in fuzzyDataSet
		int dimensionsCount;
		
		std::vector<int> dataSetParams;		

		std::vector<ClusteringModel*> clusteringModels;

		std::vector<std::vector<ElemCondClustering*> > elemCond;
	
		// List of clustering features using for transform strip to vector of features
		std::list<ClusteringFeature*> clusteringFeatures;

		// Count of features in list "clusteringFeatures"
		int featuresCount;

		// count of strips which are randomly taken from trajectory
		int stripsCount;

		int stripLength;				
	};
};

};

#endif //FUZZYMULTIDATAEXT_EC_CLUSTERING_CONFIGURATION_H
