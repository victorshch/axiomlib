#include <boost/algorithm/string.hpp>

#include "AxiomLibException.h"
#include "LabelingStrategySimple.h"

#include "LabelingStrategyFactory.h"

using namespace AxiomLib;

LabelingStrategyFactory::LabelingStrategyFactory()
{
}

LabelingStrategy *LabelingStrategyFactory::create(const std::string &name) const
{
	std::string nameLower = boost::algorithm::to_lower_copy(name);
	if(nameLower == "labelingstrategysimple") {
		return new LabelingStrategySimple();
	} else {
		throw AxiomLibException("LabelingStrategyFactory::create : unknown LabelingStrategy '" + name + "'");
	}
}
