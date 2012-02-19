#include "MetricsEqual.h"

double Equal::compute(std::vector<bool>& v1, std::vector<bool>& v2);
{
    int len1=v1.size();
    int len2=v2.size();
    if (len1!=len2)
        return 1;
    for ( int i=0;i<len1;i++)
        if ( v1[i] != v2[i] )
            return 1;
    return 0;
}
