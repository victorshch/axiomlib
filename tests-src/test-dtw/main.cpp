
#include "MultiMarking/MetricsEqual.h"
#include "MultiMarking/dtwmetric.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace AxiomLib;
using namespace MultiMarking;

int main()
{
    std::vector<double> result(2);
    Equal* metric;
    MultiMark razmetka(3),etalon(3);
    // Выделение данных
    for (int i=0;i<3;i++){
        razmetka[i].resize(2);
        etalon[i].resize(2);
    }
    // Заполнение
    razmetka[0][0]=0;razmetka[0][1]=1;
    razmetka[1][0]=1;razmetka[1][1]=0;
    razmetka[2][0]=1;razmetka[2][1]=1;

    etalon[0][0]=1;etalon[0][1]=1;
    etalon[1][0]=0;etalon[1][1]=1;
    etalon[2][0]=1;etalon[2][1]=0;

    DTWMetric::computeDTW (metric, razmetka, 0,2,3, etalon, result);

    cout << result[0] << '  ' << result[1];

    // сформировать несколько входных наборов для функции comupteDTW,
    // сравнить то, что выдает эта функция с результатами, подсчитанными вручную
    cout << "Hello World!" << endl;
    return 0;
}
