/****************************************************************************
*			dtwmetric.сxx
*
*	Description: Реализация методов класса
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "dtwmetric.h"
#include <boost/algorithm/string/case_conv.hpp>

#include "AxiomLibException.h"
#include "MetricsBetweenSets1.h"
#include "MetricsBetweenSets2.h"
#include "MetricsEqual.h"
#include "MetricsEuclidean.h"
#include "MetricsHamming.h"
#include "MetricsMatrix.h"

namespace AxiomLib {

namespace MultiMarking {

    double calculate(double a,double b,double c) {
        return ((a+b)/(c+1));
    }

    void DTWMetric::computeDTW (DTWMetric* m, const MultiMark& t,int i,int Nmin,
                                int Nmax, const MultiMark& ref, std::vector<double>& result) {
        int len_t=Nmax; // Максимальный размер окна = размер матрицы
        int len_ref=ref.size(); /* Длина эталонной траектории*/

        // Инициализация матриц
        std::vector<std::vector<double>> D(len_t+1),R(len_t+1),S(len_t+1);
        for (int u=0;u<len_ref+1;u++){
            D[u].resize(len_ref+1);
            R[u].resize(len_ref+1);
            S[u].resize(len_ref+1);
        }

        // Создание матрицы расстояний
        for (int a=0 ; a < len_ref ; a++) {
            for (int b=0 ; b < len_t ; b++ ) {
                D[a][b]=m->compute ( ref[a],t[i-len_t+1+b] );
            }
        }

        // Работа с матрицами R и S
        S[len_ref-1][len_t-1]=D[len_ref-1][len_t-1];
        R[len_ref-1][len_t-1]=1;

        // Заполнение дополнительных строки и столбца
        for(int z=0;z<len_ref+1;z++) {
            S[z][len_t]=D[len_ref-1][len_t-1];
            R[z][len_t]=1;
        }

        for (int z=0;z<len_t+1;z++) {
            S[len_ref][z]=D[len_ref-1][len_t-1];
            R[len_ref][z]=1;
        }

        // Заполнение матриц
        double diag,right,down;
        for (int b=(len_t-1);b>-1;b--) {
            for (int a=(len_ref-1);a>-1;a--) {

                // В алгоритме не надо обрабатывать самую нижнюю клетку. По-моему тут это обрабатывается.
                // Подсчет diag,right,down для данного случая
                diag=calculate(D[a][b],S[a+1][b+1],R[a+1][b+1]);
                right=calculate(D[a][b],S[a][b+1],R[a][b+1]);
                down=calculate(D[a][b],S[a+1][b],R[a+1][b]);
                if ((down<right)&&(down<diag)) {
                    S[a][b]=D[a][b]+S[a+1][b];
                    R[a][b]=R[a+1][b]+1;
                }
                if ((diag<=right)&&(diag<=down)) {
                    S[a][b]=D[a][b]+S[a+1][b+1];
                    R[a][b]=R[a+1][b+1]+1;
                }
                if ((right<diag)&&(right<down)) {
                    S[a][b]=D[a][b]+S[a][b+1];
                    R[a][b]=R[a][b+1]+1;
                }
            }
        }

        for (int j=0;j<(Nmax-Nmin+1);j++) {
            result[j]=(S[0][j]/R[0][j]);
        }
    }

    void DTWMetric::computeDTWForMetric(const MultiMark &t, int i, int Nmin, int Nmax, const MultiMark &ref, std::vector<double> &result){
        computeDTW(this, t, i, Nmin, Nmax, ref, result);
    }


    DTWMetric* DTWMetric::getMetric(std::string name){
        // преобразование в строчные буквы
          std::string nameCopy = boost::algorithm::to_lower_copy(name);
          DTWMetric* result;
          if(nameCopy == "betweensets1") {
              result=new BetweenSets1;
              return result;
          }
          if(nameCopy == "betweensets2") {
              result=new BetweenSets2;
              return result;
          }
          if(nameCopy == "equal") {
              result=new Equal;
              return result;
          }
          if(nameCopy == "euclidean") {
              result=new Euclidean;
              return result;
          }
          if(nameCopy == "hamming") {
              result=new Hamming;
              return result;
          }
          if(nameCopy == "matrix") {
              result=new Matrix;
              return result;
          }
          throw AxiomLibException("Can not find a name of metric");
    }


};//  end of namespace MultiMraking

};//  end of namespace AxiomLib
