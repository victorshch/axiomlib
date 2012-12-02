#include "ASStage.h"

#include <boost/algorithm/string.hpp>

#include "AXStage.h"
#include "ASStageSimple.h"
#include "ASStageMultiMarking.h"

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

ASStage *ASStage::create(const std::string &name, FuzzyDataSet *fuzzyDataSet, AXStage *stage2)
{
	std::string nameCopy = boost::algorithm::to_lower_copy(name);
	if(nameCopy == "simple") {
		return new ASStageSimple(fuzzyDataSet, stage2);
	}
	
        if(nameCopy == "multimarking") {
                return new ASStageMultiMarking();
        }
	return 0;
}
