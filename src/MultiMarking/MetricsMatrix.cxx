/****************************************************************************
*			MetricsMatrix.сxx
*
*	Description: Реализация метода класса
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "MetricsMatrix.h"


namespace AxiomLib {

namespace MultiMarking {

    double Matrix::compute(const std::vector<bool>& v1, const std::vector<bool>& v2) {
        int indicator=0;
        std::vector<bool> s=v1;
        for (int i=0;i<s.size();i++) {
            if (v1[i]!=v2[i]) {
                indicator=1;
            }
        }
        return indicator;
    }

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib


