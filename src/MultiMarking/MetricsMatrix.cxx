#include "MetricsMatrix.h"

double Matrix::compute(const std::vector<bool>& v1, const std::vector<bool>& v2);
{
    indicator=0;
    vector s=v1;
    for (i=0;i<s.size();i++)
        if (v1[i]!=v2[i])
            indicator=1;
    return indicator;
}
