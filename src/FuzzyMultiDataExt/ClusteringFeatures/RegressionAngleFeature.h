#ifndef REGRESSIONANGLECLUSTERINGFEATURE_CPP
#define REGRESSIONANGLECLUSTERINGFEATURE_CPP

#include "../../Environment.h"
#include "ClusteringFeature.h"
#include <vector>

namespace AxiomLib {

namespace FuzzyMultiDataExt {	
	class RegressionAngleFeature : public ClusteringFeature {
		public:
			void initFromEnv(const Environment& env) {
				// nothing
			}

			double calculate(const std::vector<double>& trajectory, int start, int size) {
				double mean_y = 0;
				double mean_x = 0;
				double sum_y = 0;
				double sum_x = 0;
				double sum_xy = 0;
				double sum_x2 = 0;

				auto st = trajectory.begin() + start;
				auto end = trajectory.begin() + start + size;
				for (int x = 0; st != end; st++, x++) 
				{
					sum_y += *st;
					sum_xy += x * (*st);
					sum_x2 += x * x;	
					sum_x += x;
				}

				mean_y = sum_y / size;
				mean_x = sum_x / size;

				double b = (sum_xy - sum_x * sum_y / (size)) / (sum_x2 - sum_x * sum_x / (size));
				
				return b;
			}	

			void setParam(std::string name, std::string value){
			}
	};
};

};

#endif //REGRESSIONANGLECLUSTERINGFEATURE_CPP