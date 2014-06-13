#ifndef MEANVALUECLUSTERINGFEATURE_CPP
#define MEANVALUECLUSTERINGFEATURE_CPP

#include "../../Environment.h"
#include "ClusteringFeature.h"
#include <vector>

namespace AxiomLib {

namespace FuzzyMultiDataExt {	
	class MeanValueFeature : public ClusteringFeature {
		public:
			void initFromEnv(const Environment& env) {
				// nothing
			}

			double calculate(const std::vector<double>& trajectory, int start, int size) {
				double mean = 0;

				for (auto st = trajectory.begin() + start, end = trajectory.begin() + start + size;
					st != end; st++) {
						mean += *st;
				}

				mean /= size;

				return mean;
			}

			void setParam(std::string name, std::string value){
			}
	};
};

};

#endif //MEANVALUECLUSTERINGFEATURE_CPP