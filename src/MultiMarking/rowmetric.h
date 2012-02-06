/****************************************************************************
*			RowMetric
*
*	Description:	RowMetric
*
*	Author:		armkor
*	History:
*
****************************************************************************/

#ifndef ROWMETRIC_H
#define ROWMETRIC_H

#include "dtwmetric.h"
#include "multimarks.h";

namespace AxiomLib {

namespace MultiMarking {

class RowMetric : public DTWMetric {

private:

 //
 RowMetric (MultiMark& s);

public:

 //
virtual void compute (MultiMark& v1, MultiMark& v2,int begin,int end, const MultiMark& ref, std::vector<double> result);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib

#endif // ROWMETRIC_H
