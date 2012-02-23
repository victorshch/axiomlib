/****************************************************************************
*			MetricsEqual.h
*
*	Description: Класс-метрика. Содержит метод - подсчет расстояния DTW
*                    Идея метрики - полное совпадение множеств.
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "dtwmetric.h"
#include "multimarks.h"

namespace AxiomLib {

namespace MultiMarking {

class Equal : public DTWMetric {

    public:

    // realisation distance for DTW
    double compute(std::vector<bool>& v1, std::vector<bool>& v2);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib
