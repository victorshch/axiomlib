#include "AsymmBetweenSets.h"

using namespace AxiomLib;
using namespace MultiMarking;

AsymmBetweenSets::AsymmBetweenSets()
{
}


double AxiomLib::MultiMarking::AsymmBetweenSets::compute(const std::vector<bool> &v1, const std::vector<bool> &v2)
{
	double s=0;
	int n=v1.size();
	int n1 = 0;
	for (int i=0;i<n;i++) {
		if(v1[i]) {
			++n1;
			if(!v2[i]) {
				++s;
			}
		}
	}
	s /= (double)n1;
	return s;
}
