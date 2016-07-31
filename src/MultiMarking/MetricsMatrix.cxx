/****************************************************************************
*			MetricsMatrix.сxx
*
*	Description: Реализация метода класса
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "MetricsMatrix.h"

#include <algorithm>

namespace AxiomLib {

namespace MultiMarking {

    double Matrix::compute(const std::vector<bool>& v1, const std::vector<bool>& v2) { // Передаем столбцы матрицы
        if ( v1[currentAxiomNumber] == v2[currentAxiomNumber] )
            return 0;
        return 1;
    }

    void Matrix::computeDTWForMetric(const MultiMark &t, int i, int Nmin, int Nmax, const MultiMark &ref, std::vector<double> &result)
    {
		result.resize(Nmax - Nmin + 1);
		std::fill(result.begin(), result.end(), 0);
		
		if(t.empty()) {
			return;
		}
		
		int axiomCount = (int) t[0].size();
		
		std::vector<double> tempResult;
		
		for(int currentAxiom = 0; currentAxiom < axiomCount; ++currentAxiom) {
			this->currentAxiomNumber = currentAxiom;
			DTWMetric::computeDTW(this, t, i, Nmin, Nmax, ref, tempResult);
			for(int i = 0; i < result.size(); ++i) {
				result[i] += tempResult[i];
			}
		}
		
		for(int i = 0; i < result.size(); ++i) {
			result[i] /= (double) axiomCount;
		}
    }

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib


