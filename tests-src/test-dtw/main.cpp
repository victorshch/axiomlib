
#include "MultiMarking/MetricsEqual.h"
#include "MultiMarking/dtwmetric.h"
#include "AxiomSet.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace AxiomLib;
using namespace MultiMarking;

int main()
{
    std::vector<double> result(2);
    Equal* metric;
    metric=new Equal;/*
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

    DTWMetric::computeDTW (metric, razmetka, 3,2,3, etalon, result);

    cout <<" Correct answer 0.5 Result:" << result[0] << "\n" <<  " Correct answer 0.(6) Result: " << result[1]<< "\n" ;

*/
    std::vector<double> result2(2);

    MultiMark razmetka2(3),etalon2(2);
    // Выделение данных
    for (int i=0;i<3;i++){
        razmetka2[i].resize(2);
        if (i!=2) etalon2[i].resize(2);
    }
    // Заполнение
    razmetka2[0][0]=0;razmetka2[0][1]=0;
    razmetka2[1][0]=0;razmetka2[1][1]=1;
    razmetka2[2][0]=1;razmetka2[2][1]=1;

    etalon2[0][0]=1;etalon2[0][1]=1;
    etalon2[1][0]=1;etalon2[1][1]=0;
/*
    for (int s=0;s<3;s++){
        for (int e=0;e<2;e++){
            cout << razmetka2[s][0] << razmetka2[s][1] << "etalon" << etalon2[e][0]<<etalon2[e][1]<<"\n";
        }
    } */

    DTWMetric::computeDTW (metric, razmetka2, 3,2,3, etalon2, result2);
    cout <<" Answer2  " << result2[0] << "\n"<< result2[1];

    MultiMark razmetka3(7),etalon3(6);
    for (int i=0;i<7;i++){
        razmetka3[i].resize(1);
    }
    razmetka3[0][0]=0;
    razmetka3[1][0]=1;
    razmetka3[2][0]=1;
    razmetka3[3][0]=2;
    razmetka3[4][0]=3;
    razmetka3[5][0]=2;
    razmetka3[6][0]=1;
    for (int i=0;i<6;i++){
        etalon3[i].resize(1);
    }
    etalon3[0][0]=1;
    etalon3[1][0]=1;
    etalon3[2][0]=2;
    etalon3[3][0]=3;
    etalon3[4][0]=2;
    etalon3[5][0]=0;

    std::vector<double> result3(1);

    DTWMetric::computeDTW (metric, razmetka3, 7 ,7,7, etalon3, result3);
    cout <<" Answer3  " << result3[0] ;
   // Тест разметки

    AxiomSet axiomSet;
    vector <MultiMarking::MultiMark> etalon_;
    std::vector<double> vec;
    vec.resize(7);
    for (int i=0;i<7;i++){
        vec[i]=i;
    }
    std::vector<bool> temp_AxiomUsage;
    temp_AxiomUsage.resize(axiomSet.size());
    axiomSet.enter(etalon_, vec , 0 , vec.size(),temp_AxiomUsage);
    std::cout << "Vivod";
    for (int i=0;i<7;i++){
        //std::cout << "\n" <<  etalon_[i][0];
    }
    return 0;
}
