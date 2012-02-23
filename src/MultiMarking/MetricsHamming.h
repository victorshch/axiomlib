/****************************************************************************
*			MetricsHamming.h
*
*	Description: Класс-метрика. Метрика - расстояние Хэмминга.
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "dtwmetric.h"
#include "multimarks.h"

namespace AxiomLib {

namespace MultiMarking {

class Hamming : public DTWMetric {

    public:

    //
    double compute(std::vector<bool>& v1, std::vector<bool>& v2);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib

