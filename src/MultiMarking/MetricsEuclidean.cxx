/****************************************************************************
*			MetricsEuclidean.сxx
*
*	Description: Реализация метода класса
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "MetricsEuclidean.h"


namespace AxiomLib {

namespace MultiMarking {

    double Euclidean::compute(std::vector<bool>& v1, std::vector<bool>& v2) {
        double s=0.0;
        int n=v1.size();
        for (int i=0;i<n;i++) {
            if ((v1[i] ^ v2[i]) && (v1[i] ^ v2[i])) {
                s++;
            }
        }
        s=sqrt(s);
        s=s / sqrt(v1.size()) ;
        return s;
    }

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib


