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
		//return new ECStageClustering(fuzzyDataSet, stage0);
	}
	
	return 0;
}
