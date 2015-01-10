#ifndef MINCLUSTERINGFEATURE_CPP
#define MINCLUSTERINGFEATURE_CPP

#include "../../Environment.h"
#include "ClusteringFeature.h"
#include <vector>

namespace AxiomLib {

namespace FuzzyMultiDataExt {	
	class MinValueFeature : public ClusteringFeature {
		public:
			void initFromEnv(const Environment& env) {
				// nothing
			}

			double calculate(const std::vector<double>& trajectory, int start, int size) {
				return *std::min_element(trajectory.begin() + start, trajectory.begin() + start + size);
			}

			void setParam(std::string name, std::string value){
			}
        private:
           friend class boost::serialization::access;

           template<class Archive>
           void serialize(Archive &, const unsigned int) const
           {
               boost::serialization::base_object<ClusteringFeature>(*this);
           }
	};
};

};

#endif //MINCLUSTERINGFEATURE_CPP
