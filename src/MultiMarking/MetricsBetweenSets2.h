/****************************************************************************
*			Metric2
*
*	Description:	Metric2
*
*	Author:		armkor
*	History:
*
****************************************************************************/

#ifndef METRIC2_H
#define METRIC2_H

#include "dtwmetric.h"
#include "multimarks.h"

namespace AxiomLib {

namespace MultiMarking {

class BetweenSets2 : public DTWMetric {

private:

    // creation Metric
    BetweenSets2();

    public:

    // distance for DTW
    double compute(std::vector<bool>& v1, std::vector<bool>& v2);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib

#endif // METRIC2_H
