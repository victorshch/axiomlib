/****************************************************************************
*			Metric4
*
*	Description:	Metric4
*
*	Author:		armkor
*	History:
*
****************************************************************************/

#ifndef METRIC4_H
#define METRIC4_H

#include "dtwmetric.h"
#include "multimarks.h";

namespace AxiomLib {

namespace MultiMarking {

class Hamming : public DTWMetric {

private:

    //
    Hamming();

    public:

    //
    double compute(std::vector<bool>& v1, std::vector<bool>& v2);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib

#endif // METRIC4_H
