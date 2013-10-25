#pragma once

#ifndef CLUSTERINGCONFIGURATION_H
#define CLUSTERINGCONFIGURATION_H

#include <string>
#include "../ClusteringRealizations/ClusteringModel.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {	

class ClusteringConfiguration {
	public:
		//static ClusteringConfiguration* Do(const std::string& args){
		//	return 
		//}

		void train(const std::vector<std::vector<double> >& data){
			clusteringModel -> addElements(data);
		}


protected: 
	ClustringModel* clusteringModel;
};

};

};

#endif