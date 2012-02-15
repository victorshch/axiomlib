#include "MetricsHamming.h"

double Hamming::compute(std::vector<bool>& v1, std::vector<bool>& v2);
{
    double s=0;
    int n=v1.size();
    for (int i=0;i<n;i++)
        s=s+(v1[i]-v2[i])*(v1[i]-v2[i]);
    return s;
}
