#ifndef ASYMMHAMMING_H
#define ASYMMHAMMING_H

#include "dtwmetric.h"

namespace AxiomLib {

namespace MultiMarking {

class AsymmHamming : public DTWMetric
{
public:
	AsymmHamming();

	virtual double compute(const std::vector<bool> &v1, const std::vector<bool> &v2);
};

}

}

#endif // ASYMMHAMMING_H
