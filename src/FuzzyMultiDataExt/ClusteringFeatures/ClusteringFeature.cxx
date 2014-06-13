#include "ClusteringFeature.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "MaxValueFeature.h"
#include "MinValueFeature.h"
#include "MeanValueFeature.h"
#include "StandardDeviationFeature.h"
#include "RegressionErrorFeature.h"
#include "RegressionAngleFeature.h"

using namespace std;
using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

ClusteringFeature* getFeatureByName(string name){
	if (name == "max")
	{
		return new MaxValueFeature();
	} else if (name == "min")
	{
		return new MinValueFeature();
	} else if (name == "mean"){
		return new MeanValueFeature();
	} else if (name == "standartdeviation"){
		return new StandardDeviationFeature();
	} else if (name == "regressionerror"){
		return new RegressionErrorFeature();
	} else if (name == "regressionangle"){
		return new RegressionAngleFeature();
	}

	throw AxiomLibException("Feature with name \"" + name + "\" not found!");
}

ClusteringFeature* ClusteringFeature::create(const std::string& s){
	if (s.empty()) {
		throw AxiomLibException("ClusteringFeature::create string s in empty.");			
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
				throw AxiomLibException("ClusteringFeature::create. Parameter error.");
			}

			boost::trim(param[0]);
			boost::trim(param[1]);
			feature->setParam(param[0], param[1]);
		}
	}
	return feature;
}