/****************************************************************************
*			MetricsBetweenSets2.h
*
*	Description: Класс-метрика. Содержит метод - подсчет расстояния
*                    между двумя множествами, посредством метрики, основанной
*                    на поиске одинаковых элементов в двух множествах, без
*                    учета дополнительной информации.
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "dtwmetric.h"
#include "multimarks.h"

namespace AxiomLib {

namespace MultiMarking {

class BetweenSets2 : public DTWMetric {

    public:

    BetweenSets2() {}

    // distance for DTW
    double compute(const std::vector<bool>& v1, const std::vector<bool>& v2);

    void computeDTWForMetric (const MultiMark& t, int i,int Nmin,int Nmax, const MultiMark& ref, std::vector<double>& result);

};// end of class

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib

