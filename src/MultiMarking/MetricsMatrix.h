/****************************************************************************
*			MetricsMatrix.h
*
*	Description: Класс-метрика. Метрика - расстояние между строками матриц.
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "dtwmetric.h"
#include "multimarks.h"

namespace AxiomLib {

namespace MultiMarking {

class Matrix : public DTWMetric {

public:
 //
double compute (const std::vector<bool>& v1, const std::vector<bool>& v2);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib

