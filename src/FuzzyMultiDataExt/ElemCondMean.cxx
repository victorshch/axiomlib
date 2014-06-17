#include "ElemCondMean.h"

#include "boost/accumulators/accumulators.hpp"
#include "boost/accumulators/statistics/mean.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/extended_type_info.hpp>
#include <boost/serialization/export.hpp>

#include <boost/lexical_cast.hpp>

#include "../Logger.h"

namespace AxiomLib {

ElemCondMean::ElemCondMean()
	: left(0), right(0), minLevel(0.0), maxLevel(1.0)
{ }

ElemCondMean::ElemCondMean(int left, int right, double minLevel, double maxLevel)
	: left(left), right(right), minLevel(minLevel), maxLevel(maxLevel)
{ }

std::string ElemCondMean::name() const
{
	return "ElemCondMean";
}

signed int ElemCondMean::check(const unsigned long k, const std::vector<double> &x) const
{
	boost::accumulators::accumulator_set<double,
		boost::accumulators::features<boost::accumulators::tag::mean> > stats;

	// проверка границ
	if (( (int) k < left) || ( (int) (x.size() - k) <= right)) {
		return -1;
	}
	// сбор статистики
	for (int i = (int) k - left; i <= (int) k + right; i++) {
		stats(x[i]);
	}

	double meanValue = boost::accumulators::mean(stats);
	if ((meanValue <= maxLevel) && (meanValue >= minLevel)) {
		return 1;
	} else {
		return 0;
	}
}

signed int ElemCondMean::getParams(std::map<std::string, std::string> &newParams) const
{
	newParams.clear();
	newParams["left"] = boost::lexical_cast<std::string>(left);
	newParams["right"] = boost::lexical_cast<std::string>(right);
	newParams["minLevel"] = boost::lexical_cast<std::string>(minLevel);
	newParams["maxLevel"] = boost::lexical_cast<std::string>(maxLevel);
}

}
