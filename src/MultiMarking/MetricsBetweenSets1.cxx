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
    //std::cout << "BetweenSets1::compute";
    double k=0; /* Расстояние между двумя множествами */
    int min=0; /* Мощность меньшего множества */
    int max=0; /* Мощность большего множества */
    for (int i=0;i < v1.size();i++) {
      //  std::cout <<"v1[i]"<< v1[i];
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
          int t = min;
          min = max;
          max = t;
      }

    if (max==0) {
        return 0;
    }
    for (int i=0;i < min;i++) {
        //std::cout <<"v1[i]"<< v1[i] << "v2[i]"<<v2[i] << "\n";
        if (v1[i] == v2[i]) {
            k++;
        }
    }
    k=1-(k / (double)max);
    return k;
}
void BetweenSets1::computeDTWForMetric (const MultiMark& t, int i,int Nmin,int Nmax, const MultiMark& ref, std::vector<double>& result){
    DTWMetric::computeDTW (this,t,i,Nmin,Nmax, ref, result);
}


};//  end of namespace MultiMraking

};//  end of namespace AxiomLib
