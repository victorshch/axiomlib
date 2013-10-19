#ifndef CLUSTERINGFEAUTUREFACTORY_H
#define CLUSTERINGFEAUTUREFACTORY_H

#include "../../Environment.h"
#include <vector>
#include "MaxClusteringFeature.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {	
class ClusteringFeatureFactory {			
	public:
		static ClusteringFeauture* Create(std::string name, std::string params){
			// TODO: сделать фабрику
			return new MaxValueFeature();
		}
}
};

};

#endif //CLUSTERINGFEAUTUREFACTORY_H