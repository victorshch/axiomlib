#ifndef WEAKEQUAL_H
#define WEAKEQUAL_H

#include "dtwmetric.h"

namespace AxiomLib {

namespace MultiMarking {

class WeakEqual : public DTWMetric
{
public:
	WeakEqual();

	virtual DTWMetric* clone() const { return DTWMetric::clone_impl(this); }

	virtual double compute(const std::vector<bool> &v1, const std::vector<bool> &v2);
};

}

}

#endif // WEAKEQUAL_H
