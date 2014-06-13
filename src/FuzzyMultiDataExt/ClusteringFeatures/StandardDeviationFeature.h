#ifndef STANDARTDEVIATIONCLUSTERINGFEATURE_CPP
#define STANDARTDEVIATIONCLUSTERINGFEATURE_CPP

#include "../../Environment.h"
#include "ClusteringFeature.h"
#include <vector>

namespace AxiomLib {

namespace FuzzyMultiDataExt {	
	class StandardDeviationFeature : public MeanValueFeature {
		public:
			void initFromEnv(const Environment& env) {
				// nothing
			}

			double calculate(const std::vector<double>& trajectory, int start, int size) {
				double meanValue = MeanValueFeature::calculate(trajectory, start, size);
				
				double sum = 0;

				for (auto st = trajectory.begin() + start, 
					end = trajectory.begin() + start + size; st != end; st++) 
				{
					sum = (*st - meanValue) * (*st - meanValue);
				}

				return sqrt(sum / size);
			}

			void setParam(std::string name, std::string value){
			}
	};
};

};

#endif //STANDARTDEVIATIONCLUSTERINGFEATURE_CPP