#ifndef DICE_H
#define DICE_H

#include "dtwmetric.h"

namespace AxiomLib {

namespace MultiMarking {

class Dice : public DTWMetric
{
public:
	Dice();
	
	virtual double compute(const std::vector<bool>& v1, const std::vector<bool>& v2);
};

}

}

#endif // DICE_H
