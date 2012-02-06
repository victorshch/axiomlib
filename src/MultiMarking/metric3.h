/****************************************************************************
*			Metric3
*
*	Description:	Metric3
*
*	Author:		armkor
*	History:
*
****************************************************************************/

#ifndef METRIC3_H
#define METRIC3_H

#include "dtwmetric.h"
#include "multimarks.h";

namespace AxiomLib {

namespace MultiMarking {

class Metric3 : public DTWMetric {

private:

    //
    Metric3 (MultiMark& f);

    public:

    // distance for DTW
    double compute(std::vector<bool>& v1, std::vector<bool>& v2);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib

#endif // METRIC3_H
