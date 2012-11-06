#include "WeakEqual.h"

using namespace AxiomLib;
using namespace MultiMarking;

WeakEqual::WeakEqual()
{
}

double WeakEqual::compute(const std::vector<bool> &v1, const std::vector<bool> &v2)
{
	for(std::vector<bool>::size_type i = 0; i < v1.size(); ++i) {
		if (v1[i] == v2[i]) {
			return 0;
		}
	}

	return 1;
}
