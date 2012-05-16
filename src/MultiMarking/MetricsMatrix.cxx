/****************************************************************************
*			MetricsMatrix.сxx
*
*	Description: Реализация метода класса
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "MetricsMatrix.h"
#include "dtwmetric.h"

namespace AxiomLib {

namespace MultiMarking {

    double Matrix::compute(const std::vector<bool>& v1, const std::vector<bool>& v2) { // Передаем столбцы матрицы
        if ( v1[currentAxiomNumber] == v2[currentAxiomNumber] )
            return 1;
        return 0;
    }

    void Matrix::computeDTWForMetric(const MultiMark &t, int i, int Nmin, int Nmax, const MultiMark &ref, std::vector<double> &result)
    {
        MultiMark temp;
        int y=result.size(); // Узнаем кол-во аксиом
        std::vector<double> tempresult(y); // Промежуточный вектор значений DTW для каждой из строк
        for (int e=0;e<y;e++){ // Обнуление суммы DTW
            result[e]==0;
            tempresult[e]==0;
        }
        for (int k=0;k<y;k++){
            temp[0] = ref[k];
            this->currentAxiomNumber=k; // Устанавливаем номер рассматриваемой строки.
            DTWMetric::computeDTW (this,t,i,Nmin,Nmax,temp,tempresult); // Вызываем функцию для текущей строки
            for (int e=0;e<y;e++){
                result[e]+=tempresult[e]/y;
            }
        }

        // в цикле для каждого значения currentAxiomNumber вызвать computeDTW,
        // затем сложить результаты
    }

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib


