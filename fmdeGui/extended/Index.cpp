#include <algorithm>
#include <iostream>

#include "Index.h"

Index makeIndex(int i, int j, int k, int l) {
	Index result;
	result[0] = i;
	result[1] = j;
	result[2] = k;
	result[3] = l;
	return result;
}

Index makeIndex(int i, int j) {
	return makeIndex(i, j, -1, -1);
}

bool operator==(const Index& i1, const Index& i2) {
	return std::equal(i1.begin(), i1.end(), i2.begin());
}

std::ostream& operator<<(std::ostream& ostr, const Index& index) {
	ostr<<"("<<index[0];
	std::for_each(index.begin() + 1, index.end(), [&ostr](int i) { ostr<<", "<<i; });
	ostr<<")";
	return ostr;
}
