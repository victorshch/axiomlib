#include "MetricsBetweenSets1.h"


// realisation distance for DTW
double BetweenSets1::compute(std::vector<bool>& v1, std::vector<bool>& v2);
{
    double k=0;
    int min=v1.size();
    int max=v2.size(); // максимальная длина из двух векторов
    if (max < min)
            max=min;
    if (max==0)
        return 0;
    for(int i=0;i<min;i++)
        if (v1[i]==v2[i])
            k++;
    k=1-(k/(double)max);
    return k;
}
