//#include "../elem_conditions/Integral.h"

#include "ECStageDiscretization.h"

#include "ElemCondMean.h"

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
	env.getMandatoryParamValue(numOfLevels, "ECStageDiscretization_numOfLevels");
	env.getMandatoryParamValue(windowSize, "ECStageDiscretization_windowSize");
}

ECSelection makeECSelection(const ElemCondPlus& ec) {
	return ECSelection(ElemCondPlusStat(ec));
}

void ECStageDiscretization::run()
{
	debug() << "Starting ECStageDiscretization";

	int dimensionCount = (int) fuzzyDataSet->getParamNums().size();
	// Create (numOfLevels + 1) ECs for each dimension

	elemConditions.resize(fuzzyDataSet->getClassCount());

	for(unsigned classNo = 0; classNo < elemConditions.size(); ++classNo) {
		elemConditions[classNo].resize(dimensionCount);
		for(int dimensionIndex = 0; dimensionIndex < dimensionCount; ++dimensionIndex) {
			DiscretizationConfiguration conf(fuzzyDataSet, dimensionIndex, numOfLevels, windowSize);
			std::vector<ElemCondPlus> elemConds = conf.getElemConds();
			std::vector<ECSelection> selections;
			std::transform(elemConds.begin(), elemConds.end(), std::back_inserter(selections), &makeECSelection);
			elemConditions[classNo][dimensionIndex].push_back(selections);
		}
	}

	debug() << "ECStageDiscretization finished";
}


DiscretizationConfiguration::DiscretizationConfiguration(const FuzzyDataSet *fuzzyDataSet, int dimensionIndex, int numOfLevels, int windowSize)
	: fuzzyDataSet(fuzzyDataSet),
	  dimensionIndex(dimensionIndex),
	  numOfLevels(numOfLevels),
	  windowSize(windowSize),
	  rollingMean(boost::accumulators::tag::rolling_window::window_size = windowSize),
	  min(0), max(0)
{
}

std::vector<ElemCondPlus> DiscretizationConfiguration::getElemConds()
{
	std::vector<double> v;
	int dimension = fuzzyDataSet->getParamNums()[dimensionIndex];

	debug() << "Processing discretization configuration for dimension " << fuzzyDataSet->getParamNames()[dimension];
	debug() << "Starting statistics accumulation...";

	for(int classNo = -1; classNo < fuzzyDataSet->getClassCount(); ++classNo) {
		for(int multiTSNo = 0; multiTSNo < fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Reference, classNo); ++multiTSNo) {
			fuzzyDataSet->getTSByIndex(FuzzyDataSet::Reference, v, classNo, multiTSNo, dimension);
			accumulate(v);
		}
	}

	debug() << "Computing quantiles...";
	std::vector<double> quantiles = getQuantiles();

	debug() << "Quantiles computed";
	if(quantiles.size() != numOfLevels - 1) {
		throw AxiomLibException("DiscretizationConfiguration::getElemConds(): invalid size of quantiles vector: "
								+ boost::lexical_cast<std::string>(quantiles.size())
								+ ", num of levels: "
								+ boost::lexical_cast<std::string>(numOfLevels));
	}

	std::vector<ElemCondPlus> result;
	result.reserve(numOfLevels);

	debug() << "Creating ECs for levels";

	debug() << "Creating EC for levels " << min - 1 << ", " << quantiles[0];
	result.push_back(createECForLevels(min - 1, quantiles[0]));

	for(int level = 1; level < numOfLevels - 1; ++level) {
		debug() << "Creating EC for levels " << quantiles[level - 1] << ", " << quantiles[level];
		result.push_back(createECForLevels(quantiles[level - 1], quantiles[level]));
	}

	debug() << "Creating EC for levels " << quantiles[numOfLevels - 2] << ", " << max + 1;
	result.push_back(createECForLevels(quantiles[numOfLevels - 2], max + 1));

	return result;
}

void DiscretizationConfiguration::accumulate(const std::vector<double> &v)
{
	// reset rolling mean
	rollingMean = boost::accumulators::accumulator_set<double,
			boost::accumulators::features<boost::accumulators::tag::rolling_mean> >
			(boost::accumulators::tag::rolling_window::window_size = windowSize);

	for(unsigned i = 0; i < v.size(); ++i) {

		if(min > v[i]) min = v[i];
		if(max < v[i]) max = v[i];

		rollingMean(v[i]);
		if(i >= windowSize) {
			double currentRollingMean = boost::accumulators::rolling_mean(rollingMean);
			stats(currentRollingMean);
		}
	}
}

std::vector<double> DiscretizationConfiguration::getQuantiles() const
{
	double mean = boost::accumulators::mean(stats);
	double variance = boost::accumulators::variance(stats);

	boost::math::normal_distribution<double> norm(mean, std::sqrt(variance));
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
	ElemCondition* elemCondition = createMeanEC(windowSize / 2, windowSize / 2 - (windowSize + 1) % 2, min, max);

	ElemCondPlus result;

	// now result owns elemCondition
	result.elemCondition = elemCondition;

	result.sign = true;
	result.dimension = fuzzyDataSet->getParamNums()[dimensionIndex];

	return result;
}

ElemCondition *DiscretizationConfiguration::createMeanEC(int left, int right, double minMeanValue, double maxMeanValue)
{
	return new ElemCondMean(left, right, minMeanValue, maxMeanValue);
}
