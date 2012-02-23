/****************************************************************************
*			dtwmetric.сxx
*
*	Description: Реализация методов класса
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "dtwmetric.h"


namespace AxiomLib {

namespace MultiMarking {

    double calculate(double a,double b,double c) {
        return ((a+b)/(c+1));
    }

    void DTWMetric::computeDTW (DTWMetric* m, const MultiMark& t,int i,int Nmin,
                                int Nmax, const MultiMark& ref, std::vector<double> result) {
        // Инициализация матриц
        double **D;
        double **R;
        double **S;
        double *temp;
        int len_ref=ref.size(); /* Длина эталонной траектории*/
        temp=(double*)malloc((Nmax+1)*sizeof(double));
        D=(double**)malloc((len_ref+1)*sizeof(temp));
        R=(double**)malloc((len_ref+1)*sizeof(temp));
        S=(double**)malloc((len_ref+1)*sizeof(temp));

        // Цикл для разных длин окна
        for (int j=0;j<(Nmax-Nmin+1);j++) {
            int len_t=Nmax-j; /* Длина части разметки рассматриваемой траектории*/

            // Создание матрицы расстояний
            for (int b=0 ; b < len_t ; b++ ) {
                for (int a=0 ; a < len_ref ; a++) {
                    D[a][b]=m->compute ( ref[a],t[i-len_t+1+b] );
                }
            }

            // Создание матриц R и S
            S[len_ref][i-j]=D[len_ref-1][len_t-1];
            R[len_ref][i-j]=1;

            // Заполнение дополнительных строки и столбца
            for(int z=0;z<len_ref+1;z++) {
                S[z][len_t]=D[len_ref-1][len_t-1];
                R[z][len_t]=1;
            }
            for (int z=0;z<len_t+1;z++) {
                S[len_ref][z]=D[len_ref-1][len_t-1];
                R[len_ref][z]=1;
            }

            double diag,right,down;
            for (int b=i;b>(i-Nmax+j);b--) {
                for (int a=(len_ref-1);a>-1;a--) {

                    // Проверка на нижний правый элемент матриц
                    if ((b!=i)&&(a!=(len_ref-1))) {

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
            }
            result[j]=(S[0][0]/R[0][0]);
        }
    }


};//  end of namespace MultiMraking

};//  end of namespace AxiomLib
