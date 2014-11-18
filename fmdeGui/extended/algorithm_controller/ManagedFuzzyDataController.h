/****************************************************************************
 *			ManagedFuzzyDataController
 *
 *	Description:	класс, содержащий логику обмена информацией с GUI
 *	Author:		wictor
 *	History:	
 *
 ****************************************************************************/

#ifndef MANAGEDFUZZYDATACONTROLLER_H_
#define MANAGEDFUZZYDATACONTROLLER_H_

#include <string>
#include <list>
#include <cstring>
#include <boost/lexical_cast.hpp>
#include <fstream>

#include "ManagedFuzzyDataStage0.h"
#include "ManagedFuzzyDataStage1.h"
#include "ManagedFuzzyDataStage2.h"
#include "ManagedFuzzyDataStage3.h"
#include "FuzzyMultiDataExtAlgorithm.h"
#include "Environment.h"
#include "Logger.h"
#include "ElemConditionsFactory.h"
#include "../QtGuiException.h"

#include "statistics.h"


template<class T>
class IdentityFunctor {
public:
	T operator()(const T& value) {
		return value;
	}
};


class ManagedFuzzyDataController {
	static const char * defaultConfigFile;
	static const double maxECFactor;

	template<class ForwardIterator, class ConvertFunctor>
	void makeArgv(const std::string &processName,
			ForwardIterator begin,
			ForwardIterator end,
			ConvertFunctor convert = IdentityFunctor<std::string>());

	int argc;
	char** argv;
	int size, rank;

	std::string workDir;

	int stage;
	
	mutable int numDimensions_;

	void applyWorkDir(const std::string &workDir, const std::string paramName);
	
	void recomputeMatterECFunc();
	
	ManagedFuzzyDataController();
	
	static ManagedFuzzyDataController* self;
public:
	AxiomLib::FuzzyMultiDataExtAlgorithm fuzzyMultiDataLearnAlgorithm;
	AxiomLib::Environment env;
	
	static ManagedFuzzyDataController* getInstance() {
		if(self == 0) {
			self = new ManagedFuzzyDataController();
		}
		
		return self;
	}
	
	//Стадии алгоритма
	ManagedFuzzyDataStage0 stage0;
	ManagedFuzzyDataStage1 stage1;
	ManagedFuzzyDataStage2 stage2;
	ManagedFuzzyDataStage3 stage3;
	
	ManagedFuzzyDataStage &getStage(int stageNumber);
	
	int numDimensions() const;
	
	AxiomLib::IntInterval getClippingInterval(int classNo, int multiTSNo) const;
	void setClippingInterval(const AxiomLib::IntInterval &interval, int classNo, int multiTSNo);
	
	AxiomLib::IntInterval getRange(int classNo, int multiTSNo) const;

	std::string ecFolder() const;
	std::string asFolder() const;

	std::vector<std::string> paramNames() const { return fuzzyMultiDataLearnAlgorithm.getDataSet().getParamNames(); }
	
	int getTotalAbnormalTrajNumber() const;

	template<class ForwardIterator, class ConvertFunctor>
	void readEnv(ForwardIterator begin, ForwardIterator end, ConvertFunctor convert);
	void init(const std::string &workDir);

	void createStage0();
	void loadStage(int number, const std::string &fileName);
	void saveStage(int number, const std::string &fileName);
	void saveStageAxiomLib(int number, const std::string &folder);
	void applyStage0();
	void runLevel1();
	void createStage1();
	void applyStage1();
	void runLevel2();
	void createStage2();
	void applyStage2();
	void runLevel3();
	void createStage3();
	void applyStage3();

	void createStage(int number);
	void runLevel(int i);
	void runHeuristics(int i);
	
	int getMultiTSCount(int classNo);
	void getMultiTS(int classNo, int tsNO, std::vector<std::vector<double> > &multiTS);
	//void getTS(int classNo, int tsNo, int dimensionNo, std::vector<double> &ts);


	int getStage() const { return stage; }
	//void nextStage() { stage++; }
	void setStage(int newStage) { stage = newStage; }

	std::string getWorkDir() const { return workDir; }

	boost::shared_ptr<AxiomLib::ReducedRecognizer> createReducedRecognizer() const;
};

template<class ForwardIterator, class ConvertFunctor>
void ManagedFuzzyDataController::readEnv(ForwardIterator begin,
		ForwardIterator end,
		ConvertFunctor convert = IdentityFunctor<std::string>()) {
	try {
		makeArgv("process", begin, end, convert);
	} catch (...) {
		throw QtGuiException("Couldn't create argc&argv");
	}

	AxiomLib::Environment envStart;
	envStart.readConfigParams(begin, end, convert);

	std::string pathToConfigFile;
	if (envStart.getStringParamValue (pathToConfigFile, "ConfigFile") < 0) {
		AxiomLib::Logger::debug("ManagedFuzzyDataController::readEnv(): Config file not found in environment, defaulting to '" + std::string(defaultConfigFile) + "'");
		pathToConfigFile.assign (defaultConfigFile);
	} else {
		AxiomLib::Logger::debug("ManagedFuzzyDataController::readEnv(): Config file path: '" + pathToConfigFile + "'");
	}

	env.readConfigFile(pathToConfigFile.c_str());
	env += envStart;
}

template<class ForwardIterator, class ConvertFunctor>
void ManagedFuzzyDataController::makeArgv(const std::string &processName, ForwardIterator begin, ForwardIterator end, ConvertFunctor convert) {

	std::list<std::string> resultList;
	resultList.push_back(processName);
	int size = 1;
	while(begin != end) {
		std::string currentValue = convert(*begin);
		resultList.push_back(currentValue);
		begin++;
		size++;
	}
	argv = new char *[size];
	int i = 0;
	for(std::list<std::string>::iterator it = resultList.begin() ; it!=resultList.end(); it++) {
		argv[i] = new char[it->size()+1];
		std::strcpy(argv[i], it->c_str());
		i++;
	}

	argc = size;
}


#endif /* MANAGEDFUZZYDATACONTROLLER_H_ */
