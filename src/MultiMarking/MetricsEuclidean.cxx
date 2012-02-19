#include "MetricsEuclidean.h"

double Euclidean::compute(std::vector<bool>& v1, std::vector<bool>& v2);
{
    double s=0.0;
    int n=v1.size();
    for (int i=0;i<n;i++)
        if ((v1[i] ^ v2[i]) && (v1[i] ^ v2[i]))
            s++;
    s=std::sqrt(s);
    s=s/sqrt(v1.size());
    return s;
}
