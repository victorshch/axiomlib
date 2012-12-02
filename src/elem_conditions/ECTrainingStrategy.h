#ifndef ECTRAININGSTRATEGY_H
#define ECTRAININGSTRATEGY_H

#include <vector>

#include "../Environment.h"
#include "../ElemCondPlusStat.h"

namespace AxiomLib {

class FuzzyDataSet;

class ECTrainingStrategy
{
public:
	ECTrainingStrategy();

	virtual void initFromEnv(const Environment& env);

	virtual void trainEC(FuzzyDataSet* fuzzyDataSet, ElemCondition* ecType, int dimension, int abnormalClass,
						 std::vector<ElemCondPlusStat>& result) = 0;
};

}

#endif // ECTRAININGSTRATEGY_H
