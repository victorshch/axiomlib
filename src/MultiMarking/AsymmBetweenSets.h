#ifndef ASYMMBETWEENSETS_H
#define ASYMMBETWEENSETS_H

#include "dtwmetric.h"

namespace AxiomLib {

namespace MultiMarking {

class AsymmBetweenSets : public DTWMetric
{
public:
	AsymmBetweenSets();

	virtual double compute(const std::vector<bool> &v1, const std::vector<bool> &v2);
};

}

}

#endif // ASYMMBETWEENSETS_H
