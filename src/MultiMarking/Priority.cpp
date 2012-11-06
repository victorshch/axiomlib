#include "Priority.h"

AxiomLib::MultiMarking::Priority::Priority()
{
}


double AxiomLib::MultiMarking::Priority::compute(const std::vector<bool> &v1, const std::vector<bool> &v2)
{
	int n = v1.size();
	double result = 0;
	for(int i = 0; i < n; ++i) {
		if(v1[i]) {
			if(v2[i]) {
				break;
			} else {
				result = 1;
				break;
			}
		} else if (v2[i]) {
			result = 1;
			break;
		}
	}

	return result;
}
