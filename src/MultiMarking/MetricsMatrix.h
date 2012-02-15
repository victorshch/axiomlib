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

class Matrix : public DTWMetric {

private:

 //
 Matrix ();

public:

 //
double compute (MultiMark& v1, MultiMark& v2);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib

#endif // ROWMETRIC_H
