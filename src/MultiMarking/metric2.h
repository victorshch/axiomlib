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

class Metric2 : public DTWMetric {

private:

    // creation Metric
    Metric2(MultiMark& f);

    public:

    // distance for DTW
    double compute(std::vector<bool>& v1, std::vector<bool>& v2);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib

#endif // METRIC2_H
