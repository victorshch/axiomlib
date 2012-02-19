#include "MetricsBetweenSets1.h"

double BetweenSets1::compute(std::vector<bool>& v1, std::vector<bool>& v2)
{
    double k=0;
    int min=0;
    int max=0;

    for (int i=0;i<v1.size();i++)
        if (v1[i]==true)
            min++;
    for (int i=0;i<v2.size();i++)
        if (v2[i]==true)
            max++;
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

