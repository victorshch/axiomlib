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

class Metric4 : public DTWMetric {

private:

    //
    Metric4(MultiMark& f);

    public:

    //
    double compute(std::vector<bool>& v1, std::vector<bool>& v2);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib

#endif // METRIC4_H
