#include "Dice.h"

using namespace AxiomLib;
using namespace AxiomLib::MultiMarking;

Dice::Dice()
{
}

double Dice::compute(const std::vector<bool> &v1, const std::vector<bool> &v2)
{
	int p1 = 0, p2 = 0, pI = 0;
	
	for(int i = 0; i < (int)v1.size(); ++i) {
		if(v1[i]) {
			++p1;
		}
		if(v2[i]) {
			++p2;
		}
		if(v1[i] && v2[i]) {
			++pI;
		}
	}
	
	return 1.0 - 2.0 * (double) pI / ((double) p1 + (double) p2);
}
