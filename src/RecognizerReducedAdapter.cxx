#include <algorithm>

#include "ReducedRecognizerFactory.h"

#include "RecognizerReducedAdapter.h"

using namespace AxiomLib;

RecognizerReducedAdapter::RecognizerReducedAdapter()
{
}

int AxiomLib::RecognizerReducedAdapter::initFromEnv(const AxiomLib::Environment &env)
{
	std::string reducedRecognizerName;
	if(env.getStringParamValue(reducedRecognizerName, "ReducedRecognizer")) {
		throw AxiomLibException("RecognizerReducedAdapter::initFromEnv: ReducedRecognizer not specified");
	}
	
	ReducedRecognizerFactory factory;
	reducedRecognizer = boost::shared_ptr<ReducedRecognizer>(factory.create(reducedRecognizerName));
	return reducedRecognizer->setParamsFromEnv(env);
}

void orVector(std::vector<bool> &v1, const std::vector<bool> &v2) {
	if(v1.size() != v2.size()) {
		throw AxiomLib::AxiomLibException("orVector: vector sizes don't match");
	}
	
	for(std::vector<bool>::size_type i = 0; i < v1.size(); ++i) {
		v1[i] = v1[i] || v2[i];
	}
}

int joinResults (TrajectorySampleDistance &result, std::vector < TrajectorySampleDistance > &prelimResult) {
	// если объединять то и нечего - то просто копируем результат и выходим
	if (prelimResult.size() == 1) {
		result = prelimResult[0];
		return 0;
	}
	
	int length = prelimResult[0].length();
	int nDim = prelimResult.size();
	
	result.setLength(length);
	
	for (int i = 0; i < length; ++i) {
		for (int cl = 0; cl < result.numClasses(); ++cl) {
			double d = -1;
			for (int dim = 0; dim < nDim; ++dim) {
				double currD = prelimResult[dim].distance(cl, i);
				if(d < 0 || currD < d) {
					d = currD;
				}
			}
			result.setDistance(cl, i, d);
		}
	}
	
	return 0;
}


void RecognizerReducedAdapter::computeDistances(AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, 
											   std::vector<TrajectorySampleDistance >& result, 
											   // какие аксиомы срабатывали при разметке эталонных траекторий
											   std::vector<bool> &refAxiomUsage, 
											   // какие аксиомы срабатывали при разметке тестовых траекторий
											   std::vector<bool> &testAxiomUsage) {
	int nAxioms = axiomSet.size();
	
	refAxiomUsage.resize(nAxioms);
	std::fill(refAxiomUsage.begin(), refAxiomUsage.end(), false);
	
	testAxiomUsage.resize(nAxioms);
	std::fill(testAxiomUsage.begin(), testAxiomUsage.end(), false);
	
	int nDim = params.size();
	
	std::vector<std::vector<std::vector<int> > > refMarkup;
	
	std::vector<double> prelimResult1;
	std::vector<TrajectorySampleDistance> prelimResult2;
	
	int numOfClasses;
	std::vector<int> numOfMultiTS;
	std::vector<std::vector<int> > numOfTS;
	dataSet.getClassSize(numOfClasses, numOfMultiTS, numOfTS);
	
	refMarkup.resize(numOfClasses);
	
	int numOfTests;
	std::vector<int> numOfTestTS;
	dataSet.getTestSize(numOfTests, numOfTestTS);
	
	prelimResult2.assign(nDim, TrajectorySampleDistance(numOfClasses));
	result.assign(numOfTests, TrajectorySampleDistance(numOfClasses));
	
	std::vector<double> currTS;
	std::vector<bool> currStat;
	
	for (int cl = 0; cl < numOfClasses; cl++) {
		refMarkup[cl].resize(nDim);
		for (int dim = 0; dim < nDim; ++dim) {
			dataSet.getTSByIndexFromClass(currTS, cl, 0, params[dim]);
			axiomSet.enter(refMarkup[cl][dim], currTS, 0, currTS.size(), currStat);
			orVector(refAxiomUsage, currStat);
		}
	}
	
	std::vector<int> currTestMarkup;
	
	for (int t = 0; t < numOfTests; t++) {
		for (int dim = 0; dim < nDim; ++dim) {
			dataSet.getTSByIndexFromTests(currTS, t, params[dim]);
			axiomSet.enter(currTestMarkup, currTS, 0, currTS.size(), currStat);
			orVector(testAxiomUsage, currStat);
		
			prelimResult2[dim].setLength(currTS.size());
			
			for (int cl = 0; cl < numOfClasses; cl++) {
				reducedRecognizer->run(currTestMarkup, refMarkup[cl][dim], prelimResult1);
				prelimResult2[dim].setDistancesForClass(cl, prelimResult1);
			}
		}
		
		joinResults(result[t], prelimResult2);
	}
}

Recognizer *RecognizerReducedAdapter::copy()
{
	return new RecognizerReducedAdapter(*this);
}
