//#include "../elem_conditions/Integral.h"

#include "ECStageDiscretization.h"

#include "../undefs.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>

#include <boost/math/distributions.hpp>
#include <boost/random.hpp>

#include "../undefs.h"

#include <limits>

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

ECStageDiscretization::ECStageDiscretization(FuzzyDataSet *fuzzyDataSet, ECTypeStage *stage0)
	: ECStage(fuzzyDataSet, stage0)
{

}

int ECStageDiscretization::initFromEnv(const Environment &env)
{
	// TODO numOfLevels, windowSize
}

void ECStageDiscretization::run()
{
	// Create (numOfLevels + 1) ECs for each dimension

	// For each dimension:
	// 1. Find mean and variance of the dimension transformed by sliding window
	// 2. Create ECs according to equiprobability (assuming normal distribution)
}


DiscretizationConfiguration::DiscretizationConfiguration(FuzzyDataSet *fuzzyDataSet, int dimensionIndex, int numOfLevels, int windowSize)
	: fuzzyDataSet(fuzzyDataSet),
	  dimensionIndex(dimensionIndex),
	  numOfLevels(numOfLevels),
	  windowSize(windowSize),
	  rollingMean(boost::accumulators::tag::rolling_window::window_size = windowSize)
{
}

std::vector<ElemCondPlus> DiscretizationConfiguration::getElemConds()
{
	std::vector<double> v;
	for(int classNo = -1; classNo <= fuzzyDataSet->getClassCount(); ++classNo) {
		for(int multiTSNo = 0; multiTSNo < fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Reference, classNo); ++multiTSNo) {
			fuzzyDataSet->getTSByIndex(FuzzyDataSet::Reference, v, classNo, multiTSNo, dimensionIndex);
			accumulate(v);
		}
	}

	std::vector<double> quantiles = getQuantiles();

	if(quantiles.size() != numOfLevels - 1) {
		throw AxiomLibException("DiscretizationConfiguration::getElemConds(): invalid size of quantiles vector: "
								+ boost::lexical_cast<std::string>(quantiles.size())
								+ ", num of levels: "
								+ boost::lexical_cast<std::string>(numOfLevels));
	}

	std::vector<ElemCondPlus> result;
	result.reserve(numOfLevels);

	result.push_back(createECForLevels(std::numeric_limits<double>::min(), quantiles[0]));

	for(int level = 1; level < numOfLevels - 1; ++level) {
		result.push_back(createECForLevels(quantiles[level - 1], quantiles[level]));
	}

	result.push_back(createECForLevels(quantiles[numOfLevels - 2], std::numeric_limits<double>::max()));

	return result;
}

void DiscretizationConfiguration::accumulate(const std::vector<double> &v)
{
	for(unsigned i = 0; i < v.size(); ++i) {
		rollingMean(v[i]);
		if(i >= windowSize) {
			double rollingMean = boost::accumulators::rolling_mean(rollingMean);
			stats(rollingMean);
		}
	}
}

std::vector<double> DiscretizationConfiguration::getQuantiles() const
{
	double mean = boost::accumulators::mean(stats);
	double variance = boost::accumulators::variance(stats);

	boost::math::normal_distribution norm(mean, std::sqrt(variance));
	std::vector<double> result;
	result.reserve(numOfLevels - 1);

	for(int i = 0; i < numOfLevels - 1; ++i) {
		double p = (i + 1) / (double) numOfLevels;
		result.push_back(boost::math::quantile(norm, p));
	}

	return result;
}

ElemCondPlus DiscretizationConfiguration::createECForLevels(double min, double max)
{
	ElemCondition* elemCondition = createMeanEC(windowSize / 2, windowSize / 2 + windowSize % 2, min, max);

	ElemCondPlus result;

	// now result owns elemCondition
	result.elemCondition = elemCondition;

	result.sign = true;
	result.dimension = fuzzyDataSet->getParamNums()[dimensionIndex];
}

ElemCondition *DiscretizationConfiguration::createMeanEC(int left, int right, double minMeanValue, double maxMeanValue)
{
	// Здесь пока создаем ЭУ Integral, подставляя ему нужные параметры

	// множитель здесь не (left + right + 1) из-за особенностей ЭУ Integral (это не совсем сумма)

	// TODO написать нормальное ЭУ для среднего

	double minIntegralValue = minMeanValue * (left + right);
	double maxIntegralValue = maxMeanValue * (left + right);
	Integral* integral = new Integral(left, right, minIntegralValue, maxIntegralValue);

	return integral;
}
