#include "ClusteringFeature.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "MaxValueFeature.h"
#include "MinValueFeature.h"

using namespace std;
using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

ClusteringFeature* getFeatureByName(string name){
	if (strcmp(name.c_str(), "max") == 0)
	{
		return new MaxValueFeature();
	} else if (strcmp(name.c_str(), "min") == 0)
	{
		return new MinValueFeature();
	}

	throw AxiomLibException();
}

ClusteringFeature* ClusteringFeature::create(const std::string& s){
	if (s.empty()) {
		throw AxiomLibException();			
	}

	vector<string> strs;

	boost::split(strs, s, boost::is_any_of("|"));

	ClusteringFeature* feature;

	if (strs.size() == 0){
		feature = getFeatureByName(s);
	} else {
		feature = getFeatureByName(strs[0]);
		
		for (int i = 1; i < strs.size(); ++i){
			vector<string> param;
			boost::split(param, strs[i], boost::is_any_of("="));

			if (param.size() != 2){
				throw AxiomLibException();
			}

			boost::trim(param[0]);
			boost::trim(param[1]);
			feature->setParam(param[0], param[1]);
		}
	}
	return feature;
}