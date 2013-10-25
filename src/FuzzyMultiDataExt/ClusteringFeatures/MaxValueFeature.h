#ifndef MAXCLUSTERINGFEATURE_CPP
#define MAXCLUSTERINGFEATURE_CPP


#include "../../Environment.h"
#include "ClusteringFeature.h"
#include <vector>

namespace AxiomLib {

namespace FuzzyMultiDataExt {	
	class MaxValueFeature : public ClusteringFeature {
		public:
			void initFromEnv(const Environment& env) {
				// nothing
			}

			double calculate(const std::vector<double>& trajectory, int start, int size) {
				return *std::max_element(trajectory.begin() + start, trajectory.end() + start + size);
			}
	};
};

}

#endif //MAXCLUSTERINGFEATURE_CPP