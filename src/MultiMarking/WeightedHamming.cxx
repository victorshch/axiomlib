#include "WeightedHamming.h"

using namespace AxiomLib;
using namespace MultiMarking;


void WeightedHamming::setWeights(const std::vector<double> &weights)
{
	this->weights = weights;
}

double WeightedHamming::compute(const std::vector<bool> &v1, const std::vector<bool> &v2)
{
	double s=0;
	int n=v1.size();
	for (int i=0;i<n;i++) {
		if (v1[i]!=v2[i]) {
			s = s + weights[i];
	   }
	}
	return s;
}
