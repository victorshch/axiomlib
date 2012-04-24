/****************************************************************************
*			MetricsBetweenSets1.h
*
*	Description: Класс-метрика. Содержит метод - подсчет расстояния
*                    между двумя множествами, посредством метрики, основанной
*                    на поиске одинаковых элементов в двух множествах,
*                    с учетом дополнительной информации.
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "dtwmetric.h"
#include "multimarks.h"

namespace AxiomLib {

namespace MultiMarking {

class BetweenSets1 : public DTWMetric {

    private:

    int because_of_error;

    public:

    // realisation distance for DTW
    double compute(const std::vector<bool>& v1, const std::vector<bool>& v2);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib

