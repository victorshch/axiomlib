#include "AsymmHamming.h"

using namespace AxiomLib;
using namespace MultiMarking;

AsymmHamming::AsymmHamming()
{
}

double AsymmHamming::compute(const std::vector<bool> &v1, const std::vector<bool> &v2) {
	double s=0;
	int n=v1.size();
	for (int i=0;i<n;i++) {
		if(v1[i] && !v2[i]) {
			   s++;
		}
	}
	s /= (double)n;
	return s;
}
