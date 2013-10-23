#ifndef PRIORITY_H
#define PRIORITY_H

#include "dtwmetric.h"

namespace AxiomLib {

namespace MultiMarking {

class Priority : public DTWMetric
{
public:
	Priority();

	virtual DTWMetric* clone() const { return DTWMetric::clone_impl(this); }

	virtual double compute(const std::vector<bool> &v1, const std::vector<bool> &v2);
};

}

}

#endif // PRIORITY_H
