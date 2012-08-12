/****************************************************************************
*			MetricsHamming.сxx
*
*	Description: Реализация метода класса
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "MetricsHamming.h"


namespace AxiomLib {

namespace MultiMarking {

    double Hamming::compute(const std::vector<bool>& v1,const std::vector<bool>& v2) {
        double s=0;
        int n=v1.size();
        for (int i=0;i<n;i++) {
            if (v1[i]==v2[i]) {
               s++;
           }
        }
        s=s / v1.size();
        return s;
    }

    void Hamming::computeDTWForMetric (const MultiMark& t, int i,int Nmin,int Nmax, const MultiMark& ref, std::vector<double>& result){
        DTWMetric::computeDTW (this,t,i,Nmin,Nmax, ref, result);
    }

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib


