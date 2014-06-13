#ifndef CLUSTERINGFEAUTURE_H
#define CLUSTERINGFEAUTURE_H

#include "../../Environment.h"
#include <vector>

namespace AxiomLib {

namespace FuzzyMultiDataExt {	
class ClusteringFeature {			
	public:
		static ClusteringFeature* create(const std::string& data);

		virtual void initFromEnv(const Environment& env) = 0;
		virtual double calculate(const std::vector<double>& trajectory, int start, int size) = 0;

	protected: 
		virtual void setParam(std::string name, std::string value) = 0;
};
};

};

#endif //CLUSTERINGFEAUTURE_H