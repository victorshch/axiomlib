#ifndef WEIGHTEDHAMMING_H
#define WEIGHTEDHAMMING_H

#include "dtwmetric.h"

namespace AxiomLib {

namespace MultiMarking {

class WeightedHamming : public DTWMetric
{
	std::vector<double> weights;

public:

	virtual DTWMetric* clone() const { return DTWMetric::clone_impl(this); }

	bool isWeighted() const { return true; }

	void setWeights(const std::vector<double>& weights);

	virtual double compute(const std::vector<bool> &v1, const std::vector<bool> &v2);
};

}

}

#endif // WEIGHTEDHAMMING_H
