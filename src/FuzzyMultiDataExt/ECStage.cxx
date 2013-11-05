#include "ECStage.h"

#include <boost/algorithm/string.hpp>

#include "ECStage.h"
#include "ECStageSimple.h"
#include "ECStageClustering.h"

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

ECStage *ECStage::create(const std::string &name, FuzzyDataSet *fuzzyDataSet, ECTypeStage* stage0)
{
	std::string nameCopy = boost::algorithm::to_lower_copy(name);

	if(nameCopy == "simple") {
		return new ECStageSimple(fuzzyDataSet, stage0);
	} else if (nameCopy == "clustering"){
		return new ECStageClustering(fuzzyDataSet, stage0);
	}
	
	return 0;
}

void ECStage::checkIndices(int i, int j, int k, int l) const {
	if(i < 0 || i >= this->elemConditions.size() || j < 0 || j >= elemConditions[i].size()
			|| k < 0 || k >= elemConditions[i][j].size() || l < 0 || l >= elemConditions[i][j][k].size()) {
		throw AxiomLibException("Invalid subscript in FuzzyMultiDataExt_EC : "+str(i)+" "+str(j)+" "+str(k)+" "+str(l));
	}
}

const ElemCondPlusStat & ECStage::getEC(int aType, int dimension, int type, int n) const {
	checkIndices(aType, dimension, type, n);
	return this->elemConditions[aType][dimension][type][n].element();
}

int sizeGetter(const std::vector<ECSelection>& v) { return (int)v.size(); }

void ECStage::getECSize(std::vector<std::vector<std::vector<int> > > &result) const {
	TransformMultiVectorHelper<3>::transform(elemConditions, result, 
											 sizeGetter
	);
}

int ECStage::getECSize() const {
	return elemConditions.size();
}

int ECStage::getECSize(int aType) const {
	return elemConditions[aType].size();
}

int ECStage::getECSize(int aType, int dimension) const {
	return elemConditions[aType][dimension].size();
}

int ECStage::getECSize(int aType, int dimension, int ecType) const {
	return elemConditions[aType][dimension][ecType].size();
}

int ECStage::getECTotalCount() const
{
	int result = 0;
	
	for(int i = 0; i  < getECSize(); ++i) {
		for(int j = 0; j < getECSize(i); ++j) {
			for(int k = 0; k  < getECSize(i, j); ++k) {
				result += getECSize(i, j, k);
			}
		}
	}
	
	return result;
}

