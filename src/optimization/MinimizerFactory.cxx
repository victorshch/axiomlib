#include <boost/algorithm/string.hpp>

#include "GoldenSectionSearch.h"
#include "GridSearch.h"
#include "HybridGridSearch.h"

#include "../AxiomLibException.h"

#include "MinimizerFactory.h"

using namespace AxiomLib::Optimization;

MinimizerFactory::MinimizerFactory()
{
}

Minimizer *MinimizerFactory::create(const std::string &name)
{
	std::string nameLower = boost::algorithm::to_lower_copy(name);
	if(nameLower == "goldensectionsearch") {
		return new GoldenSectionSearch();
	} else if (nameLower == "gridsearch") {
		return new GridSearch();
	} else  if (nameLower == "hybridgridsearch") {
		return new HybridGridSearch();
	} else{
		throw AxiomLibException("MinimizerFactory::create : unknown minimizer : '" + name + "'");
	}
}
