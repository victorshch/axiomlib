/****************************************************************************
*			MetricsEqual.сxx
*
*	Description: Реализация метода класса
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "MetricsEqual.h"


namespace AxiomLib {

namespace MultiMarking {

    double Equal::compute(const std::vector<bool>& v1,const  std::vector<bool>& v2) {
        int len1=v1.size();
        int len2=v2.size();
        if (len1!=len2) {
            return 1;
        }
        for ( int i=0;i<len1;i++) {
            if ( v1[i] != v2[i] ) {
                return 1;
            }
        }
        return 0;
    }

    Equal::Equal(){ }

    void Equal::computeDTWForMetric(const MultiMark &t, int i, int Nmin, int Nmax, const MultiMark &ref, std::vector<double> &result){
        DTWMetric::computeDTW(this, t, i, Nmin, Nmax, ref, result);
    }

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib


