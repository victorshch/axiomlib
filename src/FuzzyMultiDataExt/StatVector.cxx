#include <cmath>

#include "StatVector.h"

using namespace AxiomLib::FuzzyMultiDataExt;

StatVector::StatVector(double d1, double d2, double d3, double d4, double d5)
{
	v[0] = d1;
	v[1] = d2;
	v[2] = d3;
	v[3] = d4;
	v[4] = d5;
}

double StatVector::distance(const StatVector &v1, const StatVector &v2) {
	double result = 0.0;
	
	for(int i = 0; i < statVectorDim; ++i) {
		result += std::abs(v1.v[i] - v2.v[i]);
	}
	
	return result;
}
