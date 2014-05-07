#include "ClusteringModel.h"

#include "../../AxiomLibException.h"

using namespace AxiomLib;
using namespace std;
using namespace AxiomLib::FuzzyMultiDataExt;

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "KMeansClusteringModel.h"

ClusteringModel* getModelByName(string name){
	if (strcmp(name.c_str(), "kmeans") == 0)
	{
		return new KMeansClusteringModel();
	}

	throw AxiomLibException("ClusteringModel::getModelByName. Model with name \"" + name +"\" not found!");
}

ClusteringModel* ClusteringModel::create(const string& s){
	if (s.empty()) {
		throw AxiomLibException("ClusteringModel::create. S is empty.");			
	}

	vector<string> strs;

	boost::split(strs, s, boost::is_any_of("|"));

	ClusteringModel* model;

	if (strs.size() == 0){
		model = getModelByName(s);
	} else {
		model = getModelByName(strs[0]);
		
		for (int i = 1; i < strs.size(); ++i){
			vector<string> param;
			boost::split(param, strs[i], boost::is_any_of("="));

			if (param.size() != 2){
				throw AxiomLibException("ClusetringModel::create. Parameters error.");
			}

			boost::trim(param[0]);
			boost::trim(param[1]);
			model->setParam(param[0], param[1]);
		}
	}
	return model;
}

