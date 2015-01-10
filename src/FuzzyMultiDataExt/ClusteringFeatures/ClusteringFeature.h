#ifndef CLUSTERINGFEAUTURE_H
#define CLUSTERINGFEAUTURE_H

#include "../../Environment.h"
#include <vector>

#include "boost/serialization/assume_abstract.hpp"
#include "boost/serialization/access.hpp"
#include "boost/serialization/nvp.hpp"

namespace AxiomLib {

namespace FuzzyMultiDataExt {	
class ClusteringFeature {			
	public:
		static ClusteringFeature* create(const std::string& data);

		virtual void initFromEnv(const Environment& env) = 0;
		virtual double calculate(const std::vector<double>& trajectory, int start, int size) = 0;

	protected: 
		virtual void setParam(std::string name, std::string value) = 0;

    private:
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) const { }
};
};

};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(AxiomLib::ClusteringFeature);

#endif //CLUSTERINGFEAUTURE_H
