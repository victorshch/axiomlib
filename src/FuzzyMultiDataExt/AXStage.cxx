#include <algorithm>
#include <utility>

#include <boost/algorithm/string.hpp>

#include <omp.h>

#include "../AxiomStatistics.h"
#include "../FuzzyMultiDataExtAlgorithm.h"

#include "ECStage.h"
#include "../Logger.h"
#include "Common.h"

#include "AXStageSimple.h"
#include "AXStageTree.h"
#include "AXStageCombined.h"

#include "AXStage.h"

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

AXStage *AXStage::create(std::string name, FuzzyDataSet *fuzzyDataSet, ECStage *stage1)
{
	boost::algorithm::to_lower(name);

	if(name == "simple") {
		return new AXStageSimple(fuzzyDataSet, stage1);
	}
	if(name == "tree") {
		return new AXStageTree(fuzzyDataSet, stage1);
	}
	if(name == "combined") {
		return new AXStageCombined(fuzzyDataSet, stage1);
	}

	throw AxiomLibException("AXStage::create() : unknown ASStage type : '" + name + "'");
}

AXStage::AXStage(FuzzyDataSet *fuzzyDataSet, ECStage *stage1)
	: parent(stage1->getParent()), fuzzyDataSet(fuzzyDataSet), stage1(stage1)
{

}

void AXStage::getAXSize(std::vector<int> &result) const
{
	int classCount = getAXSize();
	result.resize(classCount);

	for(int i = 0; i < classCount; ++i) {
		result[i] = getAXSize(i);
	}
}
