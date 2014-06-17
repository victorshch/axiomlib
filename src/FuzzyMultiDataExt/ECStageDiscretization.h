#ifndef ECSTAGEDISCRETIZATION_H
#define ECSTAGEDISCRETIZATION_H

#include "../undefs.h"

#include "ECStage.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include "../undefs.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class DiscretizationConfiguration {
public:
	DiscretizationConfiguration(FuzzyDataSet* fuzzyDataSet, int dimensionIndex, int numOfLevels, int windowSize);

	std::vector<ElemCondPlus> getElemConds();

private:
	FuzzyDataSet* fuzzyDataSet;

	int dimensionIndex;
	int numOfLevels;
	int windowSize;

	boost::accumulators::accumulator_set<double,
		boost::accumulators::stats<boost::accumulators::tag::rolling_mean> > rollingMean;

	boost::accumulators::accumulator_set<double,
		boost::accumulators::stats<boost::accumulators::tag::mean, boost::accumulators::tag::variance> > stats;

	// Аккумуляция среднего и среднеквадратичного отклонения для скользящего среднего от v
	void accumulate(const std::vector<double>& v);

	// Возвращает numOfLevels - 1 квантилей нормального распределения
	// для вероятностей 1/numOfLevels, 2/numOfLevels, ..., (numOfLevels - 1)/numOfLevels
	std::vector<double> getQuantiles() const;

	ElemCondPlus createECForLevels(double min, double max);

	// Создает ЭУ, выполняющееся, когда среднее от окрестности лежит в заданных диапазонах
	static ElemCondition* createMeanEC(int left, int right, double minMeanValue, double maxMeanValue);
};

class ECStageDiscretization : public ECStage
{
public:
	ECStageDiscretization(FuzzyDataSet* fuzzyDataSet, ECTypeStage* stage0);

	virtual int initFromEnv(const Environment& env);

	virtual void run();

private:
	int numOfLevels;
	int windowSize;


};

}

}

#endif // ECSTAGEDISCRETIZATION_H
