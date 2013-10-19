#ifndef CLUSTERINGFEAUTURE_H
#define CLUSTERINGFEAUTURE_H

#include "../../Environment.h"
#include <vector>

namespace AxiomLib {

namespace FuzzyMultiDataExt {	
class ClusteringFeature {			
	public:
		virtual void initFromEnv(const Environment& env) = 0;
		virtual double calculate(const std::vector<double> trajectory, int start, int size);
};
};

};

#endif //CLUSTERINGFEAUTURE_H