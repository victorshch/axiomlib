#ifndef JACCARD_H
#define JACCARD_H

#include "dtwmetric.h"

namespace AxiomLib {

namespace MultiMarking {

class Jaccard : public DTWMetric
{
public:
	Jaccard();

	virtual DTWMetric* clone() const { return DTWMetric::clone_impl(this); }
	
	virtual double compute(const std::vector<bool>& v1, const std::vector<bool>& v2);
};

}

}

#endif // JACCARD_H
