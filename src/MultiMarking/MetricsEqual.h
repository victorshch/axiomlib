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

    int s;

    public:

    Equal();

    ~Equal() { }

    // realisation distance for DTW
    double compute(const std::vector<bool>& v1, const std::vector<bool>& v2);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib
