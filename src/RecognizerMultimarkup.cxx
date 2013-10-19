#include "RecognizerMultimarkup.h"
#include "MultiMarking/dtwmetric.h"
#include "Environment.h"
#include "AxiomSet.h"
#include "DataSet.h"
#include "Logger.h"
#include <iostream>
#include <algorithm>
#include <numeric>

#include <boost/algorithm/string/case_conv.hpp>

namespace AxiomLib {

void compare (std::vector<bool>& m, const std::vector<bool> &p){
	for (int i=0;i<m.size();i++){
		if (p[i])
			m[i]=true;
	}
}

double minimum (const std::vector<double> &r){
	if(r.empty()) {
		throw AxiomLibException("minimum(): vector is empty");
	}

	double min=r[0];
	for(int i=0;i<r.size();i++){
		if (r[i]<min)
			min=r[i];
	}
	return min;
}

double average(const std::vector<double>& r) {
	if(r.empty()) {
		throw AxiomLibException("average(): vector is empty");
	}

	double sum = std::accumulate(r.begin(), r.end(), 0);

	return sum / (double) r.size();
}

void cancelAxioms(const std::vector<bool>& mask, std::vector<std::vector<bool> >& markup) {
	for(int i = 0; i < markup.size(); ++i) {
		for(int j = 0; j < mask.size(); ++j) {
			if(!mask[j]) {
				markup[i][j] = false;
			}
		}
	}
}

// Вычисление расстояний от участков разметок тестовых траекторий до разметок эталонных траекторий
void RecognizerMultiMarkup::computeDistances(AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, // номера столбцов кторые надо использовать
											 // расстояния от участков разметок тестовых траекторий до разметок эталонных траекторий
											 std::vector<TrajectorySampleDistance>& result,
											 // какие аксиомы срабатывали при разметке эталонных траекторий
											 std::vector<bool>& refAxiomUsage,
											 // какие аксиомы срабатывали при разметке тестовых траекторий
											 std::vector<bool>& testAxiomUsage) {
	MultiMarking::DTWMetric* metric = mMetric->clone();
	if(metric->isWeighted()) {
		metric->setWeights(axiomSet.getAxiomWeights());
	}

	//Logger::debug("Entering RecognizerMultiMarkup::computeDistances()");
	std::vector<bool> temp_testAxiomUsage;
	std::vector<bool> temp_refAxiomUsage;
	std::vector<std::vector<bool> > refAxiomUsageVector;
	temp_testAxiomUsage.resize(axiomSet.size(), false); // inizializacia false
	temp_refAxiomUsage.resize(axiomSet.size(), false);
	testAxiomUsage.clear();
	testAxiomUsage.resize(axiomSet.size(), false);
	refAxiomUsage.clear();
	refAxiomUsage.resize(axiomSet.size(), false);

	std::vector <MultiMarking::MultiMark> etalon(dataSet.getNumberOfClasses());
	std::vector<double> vec;
	refAxiomUsageVector.reserve(dataSet.getNumberOfClasses());
	for (int i=0;i<dataSet.getNumberOfClasses();i++){
		if (!(dataSet.getTSByIndexFromClass(vec, i , 0, params[0])))
			throw AxiomLibException("Can not create row");
		axiomSet.enter(etalon[i], vec , 0 , vec.size(), temp_refAxiomUsage);
		refAxiomUsageVector.push_back(temp_refAxiomUsage);
		compare (refAxiomUsage,temp_refAxiomUsage);
	}

	std::vector<int> numOfTS;
	int k;
	dataSet.getTestSize(k,numOfTS);
	vector <MultiMarking::MultiMark> tests (k);
	for (int i=0;i<k;i++){
		if (!(dataSet.getTSByIndexFromTests(vec, i, params[0])))
			throw AxiomLibException("Can not create row");
		axiomSet.enter(tests[i], vec , 0 , vec.size(), temp_testAxiomUsage);
		compare (testAxiomUsage,temp_testAxiomUsage);
	}

	result.reserve(tests.size());
	for(int i = 0; i < tests.size(); ++i) {
		result.push_back(TrajectorySampleDistance(dataSet.getNumberOfClasses(), tests[i].size()));
	}

	//Logger::debug("RecognizerMultiMarkup::computeDistances() : ");
	//Logger::debug("Stretch : " + boost::lexical_cast<std::string>(mStretch));
	std::vector<double> temp_result;
	MultiMarking::MultiMark tempEtalon;
	for (int j=0;j<tests.size();j++){
		result[j].setLength(tests[j].size());
		for (int i=0;i<etalon.size();i++){
			for (int s=(mStretch)*etalon[i].size();s<tests[j].size();s++){
				tempEtalon = etalon[i];
				if(mCancelAxioms) {
					//Logger::debug("Cancelling axioms...");
					cancelAxioms(refAxiomUsageVector[i], tempEtalon);
				}
				//Logger::debug("Window : " + boost::lexical_cast<std::string>((1.0/mStretch)*etalon[i].size()) + " " + boost::lexical_cast<std::string>((mStretch)*etalon[i].size()));
				//Logger::debug("Computing DTW...");
				metric->computeDTWForMetric( tests[j], s , (1.0/mStretch)*etalon[i].size(),(mStretch)*etalon[i].size(), tempEtalon, temp_result);
				//Logger::debug("Setting distance...");
				result[j].setDistance(i, s, minimum(temp_result));
			}
		}
	}
	//Logger::debug("Exiting RecognizerMultiMarkup::computeDistances()");
}

int RecognizerMultiMarkup::initFromEnv (const Environment& env) {
	if (env.getDoubleParamValue(mStretch, "stretch")!=0)
		throw AxiomLibException("Can not find stretch");
	if (env.getStringParamValue(mMetricName, "metric")!=0)
		throw AxiomLibException("Can not find the name of metric");
	bool useWrapper = false;
	if (env.getBoolParamValue(useWrapper, "cancelUnusedAxioms")) {
		useWrapper = true;
	}
	mCancelAxioms = useWrapper;
	mMetric = MultiMarking::DTWMetric::getMetric(mMetricName);
	std::string nameCopy = boost::algorithm::to_lower_copy(mMetricName);
	mMetricName=nameCopy;

	mResultStrategy = &minimum;
	std::string resultStrategyName;
	if(!env.getStringParamValue(resultStrategyName, "resultStrategy")) {
		if(resultStrategyName == "average") {
			mResultStrategy = &average;
		}
	}
}

Recognizer* RecognizerMultiMarkup::copy (void){
	return new RecognizerMultiMarkup(*this);
}

RecognizerMultiMarkup::RecognizerMultiMarkup(const RecognizerMultiMarkup& other)
	: RecognizerImplBase(other)
{
	mStretch = other.mStretch;
	mMetricName = other.mMetricName;
	mMetric = MultiMarking::DTWMetric::getMetric(other.mMetricName);
}

RecognizerMultiMarkup::~RecognizerMultiMarkup() {
	delete mMetric;
}

}

