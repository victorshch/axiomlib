#include "GridSearch.h"

using namespace AxiomLib;
using namespace AxiomLib::Optimization;

GridSearch::GridSearch()
{ }

void GridSearch::initFromEnv(const Environment &env)
{
	env.getMandatoryParamValue(numPartitions, "minimizerNumPartitions");
}

void GridSearch::findMinimum(const FunctionOneVariable *function, 
							 double lowerBound, double upperBound, 
						     double &bestX, double &bestF)
{
	double& lb = lowerBound;
	double& ub = upperBound;
	double delta = (ub - lb) / numPartitions;
	
	bestX = lb;
	bestF = function->f(lb);
	
	double currX = lb;
	for(int p = 1; p <= numPartitions; ++p) {
		currX += delta;
		double currF = function->f(currX);
		if(currF < bestF) {
			bestF = currF;
			bestX = currX;
		}
	}
}
