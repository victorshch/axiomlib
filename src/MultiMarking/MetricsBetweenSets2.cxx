/****************************************************************************
*			MetricsBetweenSets2.сxx
*
*	Description: Реализация метода класса
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "MetricsBetweenSets2.h"


namespace AxiomLib {

namespace MultiMarking {

    double BetweenSets2::compute(const std::vector<bool>& v1,const std::vector<bool>& v2) {
        double k=0;
        int min=0;
        int max=0;
        for (int i=0;i<v1.size();i++) {
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

        if (max == 0) {
            return 0;
        }
        if (min == 0) {
            return 1;
        }
        for(int i=0;i < min;i++) {
            if (v1[i] == v2[i]) {
                k++;
            }
        }
        k=1-(k / (double)min);
        return k;
    }

    void BetweenSets2::computeDTWForMetric (const MultiMark& t, int i,int Nmin,int Nmax, const MultiMark& ref, std::vector<double>& result){
        DTWMetric::computeDTW (this,t,i,Nmin,Nmax, ref, result);
    }

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib




