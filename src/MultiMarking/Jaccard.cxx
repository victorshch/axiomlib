#include "Jaccard.h"

using namespace AxiomLib;
using namespace AxiomLib::MultiMarking;

Jaccard::Jaccard()
{
}


double AxiomLib::MultiMarking::Jaccard::compute(const std::vector<bool> &v1, const std::vector<bool> &v2)
{
	int pI = 0, pU = 0;
	
	for(int i = 0; i < (int)v1.size(); ++i) {
		if(v1[i] && v2[i]) {
			++pI;
		}
		if(v1[i] || v2[i]) {
			++pU;
		}
	}
	
	return 1.0 - (double)pI / (double)pU;
}
