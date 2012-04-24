/****************************************************************************
*			MetricsBetweenSets1.сxx
*
*	Description: Реализация метода класса
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "MetricsBetweenSets1.h"

namespace AxiomLib {

namespace MultiMarking {

double BetweenSets1::compute(const std::vector<bool>& v1,const std::vector<bool>& v2) {
    double k=0; /* Расстояние между двумя множествами */
    int min=0; /* Мощность меньшего множества */
    int max=0; /* Мощность большего множества */
    for (int i=0;i < v1.size();i++) {
        if (v1[i] == true) {
            min++;
        }
    }
    for (int i=0;i<v2.size();i++) {
        if (v2[i] == true) {
            max++;
        }
    }
    if (max < min) {
        max=min;
    }
    if (max==0) {
        return 0;
    }
    for (int i=0;i < min;i++) {
        if (v1[i] == v2[i]) {
            k++;
        }
    }
    k=1-(k / (double)max);
    return k;
}



};//  end of namespace MultiMraking

};//  end of namespace AxiomLib
