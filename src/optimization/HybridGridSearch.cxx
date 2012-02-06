#include "MinimizerFactory.h"
#include "../AxiomLibException.h"

#include "HybridGridSearch.h"

using namespace AxiomLib;
using namespace AxiomLib::Optimization;

HybridGridSearch::HybridGridSearch()
{
}

void AxiomLib::Optimization::HybridGridSearch::initFromEnv(const AxiomLib::Environment &env)
{
	env.getMandatoryParamValue(numPartitions, "minimizerNumPartitions");
	if(numPartitions < 1) {
		throw AxiomLibException("HybridGridSearch::initFromEnv : invalid minimizerNumPartitions value : '"
								+boost::lexical_cast<std::string>(numPartitions) + "' (must be positive)");
	}
	
	std::string minimizerName;
	env.getMandatoryParamValue(minimizerName, "hybridLocalMinimizer");
	
	MinimizerFactory factory;
	
	minimizer.reset(factory.create(minimizerName));
	minimizer->initFromEnv(env);
}

void HybridGridSearch::findMinimum(const FunctionOneVariable *function, 
								   double lowerBound, double upperBound, 
								   double &bestX, double &bestF)
{
	double delta = (upperBound - lowerBound) / numPartitions;

	double lb = lowerBound;
	double ub = lb + delta;
	minimizer->findMinimum(function, lb, ub, bestX, bestF);
	
	double currX, currF;
	for(int i = 1; i <= numPartitions; ++i) {
		lb = ub;
		ub = lb + delta;
		minimizer->findMinimum(function, lb, ub, currX, currF);
		if(currF < bestF) {
			bestX = currX;
			bestF = currF;
		}
	}
}
