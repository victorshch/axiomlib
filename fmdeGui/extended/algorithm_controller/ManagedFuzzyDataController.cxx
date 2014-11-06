/****************************************************************************
 *			ManagedFuzzyDataController
 *
 *	Description:	реализация функций-членов
 *	Author:		wictor
 *	History:	
 *
 ****************************************************************************/

#include "IntervalSet.h"
#include "ElemConditionsFactory.h"
#include "ManagedFuzzyDataController.h"

#include "undefs.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/function.hpp>
#include <boost/lambda/bind.hpp>

//#include <boost/spirit/include/phoenix.hpp>

#include <QtDebug>

#include "../utilities.h"
#include "../QtGuiException.h"
//#include "elem_conditions/Export.h"

#include "Logger.h"

BOOST_CLASS_EXPORT_GUID(ManagedFuzzyDataStage0, "ManagedFuzzyDataStage0");
BOOST_CLASS_EXPORT_GUID(ManagedFuzzyDataStage1, "ManagedFuzzyDataStage1");

const char * ManagedFuzzyDataController::defaultConfigFile = "./axiomlib_fmd.conf";

const double ManagedFuzzyDataController::maxECFactor = 2.0;

ManagedFuzzyDataController* ManagedFuzzyDataController::self = 0;

ManagedFuzzyDataController::ManagedFuzzyDataController() : stage0(&this->fuzzyMultiDataLearnAlgorithm, &this->env),
stage1(&this->fuzzyMultiDataLearnAlgorithm, &this->env),
stage2(&this->fuzzyMultiDataLearnAlgorithm, &this->env),
stage3(&this->fuzzyMultiDataLearnAlgorithm, &this->env){
	size = 0;
	rank = 0;
	argc = 0;
	argv = 0;
	stage = 0;
	
	numDimensions_ = -1;
}

void ManagedFuzzyDataController::applyWorkDir(const std::string &workDir, std::string paramName) {
	std::string value;
	if(env.getStringParamValue(value, paramName)) {
		throw AxiomLib::AxiomLibException("applyBaseDirToName() : absent parameter " + paramName);
	}
	boost::filesystem::path path(workDir);
	boost::filesystem::path path2(value);
	if(path2.is_complete()) {
		return;
	} else {
		path /= path2;
		env.setParamValue(path.string(), paramName);
	}
}

void ManagedFuzzyDataController::init(const std::string &workDir) {
	// Добавляем рабочую директорию к BaseDataSetDir
	
	applyWorkDir(workDir, "BaseDataSetDir");

	applyWorkDir(workDir, "AxiomBaseDir");

	applyWorkDir(workDir, "AxiomSetBaseDir");

	fuzzyMultiDataLearnAlgorithm.initFromEnv(env);
	fuzzyMultiDataLearnAlgorithm.setComments(true);	
	AxiomLib::Logger::getInstance()->setDebug(true);
}

void ManagedFuzzyDataController::recomputeMatterECFunc() {
//	ECMultiVector &ecs = stage1.bestECForAllAbnormalTypes;
//	for(int i = 0; i < ecs.size(); i++) {
//		for(int j = 0; j < ecs[i].size(); j++) {
//			for(int k = 0; k < ecs[i][j].size(); k++) {
//				for(int l = 0; l < ecs[i][j][k].size(); l++) {
//					fuzzyMultiDataLearnAlgorithm.matterECFunc(
//							ecs[i][j][k][l],
//							ecs[i][j][k][l].dimension,
//							i
//							);
//				}
//			}
//		}
//	}
// TODO reimplement
}

int ManagedFuzzyDataController::numDimensions() const {
	if(numDimensions_ == -1) {
		numDimensions_ = fuzzyMultiDataLearnAlgorithm.getDataSet().getDimensionCount();
	}
	return numDimensions_;
}

ManagedFuzzyDataStage &ManagedFuzzyDataController::getStage(int stageNumber) {
	switch(stageNumber) {
	case 0: return stage0;
	case 1: return stage1; 
	case 2:return stage2;
	case 3: return stage3;
	default: throw std::logic_error("unknown stage number " + boost::lexical_cast<std::string>(stageNumber));
	}
}

//Создание дефолтных значений в ManagedFuzzyDataStage0
void ManagedFuzzyDataController::createStage0() {
	stage0.init();
	stage0.initClipping();
}

void ManagedFuzzyDataController::applyStage0() {
	stage0.save();
	stage0.saveClipping();
}

void ManagedFuzzyDataController::runLevel1() {
	fuzzyMultiDataLearnAlgorithm.runFirstLevel();

	createStage1();

	std::cout<<"Level 1 finished\n";

	setStage(1);
}

void ManagedFuzzyDataController::createStage1() {
	stage1.init();
	stage1.initClipping();
	
	//this->recomputeMatterECFunc();
}

void ManagedFuzzyDataController::applyStage1() {
	stage1.save();
	stage1.saveClipping();
}

void ManagedFuzzyDataController::runLevel2() {
	fuzzyMultiDataLearnAlgorithm.runSecondLevel();

	stage2.setCalculated(true);
}

void ManagedFuzzyDataController::createStage2() {
	stage2.init();
	stage2.initClipping();
}

void ManagedFuzzyDataController::applyStage2() {
	stage2.save();
	stage2.saveClipping();
}

void ManagedFuzzyDataController::runLevel3() {

	fuzzyMultiDataLearnAlgorithm.runThirdLevel();

	stage3.setCalculated(true);
}

void ManagedFuzzyDataController::createStage3() {
	stage3.init();
	stage3.initClipping();
}

void ManagedFuzzyDataController::applyStage3() {
	stage3.save();
	stage3.saveClipping();
}

void ManagedFuzzyDataController::createStage(int number) {
	switch(number) {
	case 0: createStage0(); break;
	case 1: createStage1(); break;
	case 2: createStage2(); break;
	case 3: createStage3(); break;
	default: throw std::logic_error("ManagedFuzzyDataController::createStage(): Unexpected level number : " + boost::lexical_cast<std::string>(number));
	}
}

void ManagedFuzzyDataController::runLevel(int levelNo) {
	switch(levelNo) {
		case 1: runLevel1(); break;
		case 2: runLevel2(); break;
		case 3: runLevel3(); break;
		default: throw std::logic_error("Unexpected level number : " + boost::lexical_cast<std::string>(levelNo));
	}
}

void ManagedFuzzyDataController::runHeuristics(int levelNo) {
	switch(levelNo) {
	case 0: fuzzyMultiDataLearnAlgorithm.runZeroLevelHeuristics(); break;
	case 1: fuzzyMultiDataLearnAlgorithm.runFirstLevelHeuristics(); break;
	case 2: fuzzyMultiDataLearnAlgorithm.runSecondLevelHeuristics(); break;
	default: throw std::logic_error("Unexpected level number : " + boost::lexical_cast<std::string>(levelNo));
	}
}

void ManagedFuzzyDataController::loadStage(int number, const std::string &fileName) {
	if(!boost::filesystem::exists(fileName)) {
		throw QtGuiException("ManagedFuzzyDataController::loadStage() : file not found : "+fileName);
	}

	std::ifstream file(fileName.c_str());
	boost::archive::xml_iarchive archive(file);

	switch (number) {
	case 0: {
		archive & BOOST_SERIALIZATION_NVP(stage0);
	} break;
	case 1: {
		archive & BOOST_SERIALIZATION_NVP(stage1);
	} break;
	case 2: {
		archive & BOOST_SERIALIZATION_NVP(stage2);
	} break;
	case 3: {
		archive & BOOST_SERIALIZATION_NVP(stage3);
	} break;
	default: throw QtGuiException(
			"ManagedFuzzyDataController::loadStage() : unexpected stage number : "
			+ boost::lexical_cast<std::string>(number)
			);
	}

}

void ManagedFuzzyDataController::saveStage(int number, const std::string &fileName) {
	 std::ofstream file(fileName.c_str());
	 boost::archive::xml_oarchive archive(file);

	switch (number) {
	case 0: {
		archive & BOOST_SERIALIZATION_NVP(stage0);
		stage0.setCalculated(true);
	} break;
	case 1: {
		archive & BOOST_SERIALIZATION_NVP(stage1);
		stage1.setCalculated(true);
	} break;
	case 2: {
		archive & BOOST_SERIALIZATION_NVP(stage2);
		stage2.setCalculated(true);
	} break;
	case 3: {
		archive & BOOST_SERIALIZATION_NVP(stage3);
		stage3.setCalculated(true);
	} break;
	default: throw QtGuiException(
			"ManagedFuzzyDataController::saveStage() : unexpected stage number : "
			+ boost::lexical_cast<std::string>(number)
			);
	}
}

void ManagedFuzzyDataController::saveStageAxiomLib(int number, const std::string &folder) {
	switch(number) {
	case 0: {
			return;
		}
	case 1: {
			if(!boost::filesystem::exists(folder)) {
				boost::filesystem::create_directory(folder);
			}
			std::string ecListFileName;
			env.getStringParamValue(ecListFileName, "ECListFileName");
			
			this->applyStage1();
			
			fuzzyMultiDataLearnAlgorithm.saveECs(folder, ecListFileName);
		}break;
	case 2: {
			if(!boost::filesystem::exists(folder)) {
				boost::filesystem::create_directory(folder);
			}
			std::string listFileName;
			env.getStringParamValue(listFileName, "AxiomListFileName");
			this->applyStage2();
			
			fuzzyMultiDataLearnAlgorithm.saveAXs(folder, listFileName);
			}break;
	case 3: {
			if(!boost::filesystem::exists(folder)) {
				boost::filesystem::create_directory(folder);
			}
			this->applyStage3();
			fuzzyMultiDataLearnAlgorithm.saveASs(folder);
		}break;
	default: throw QtGuiException(
				"ManagedFuzzyDataController::saveStageAxiomLib() : unexpected stage number : "
				+ boost::lexical_cast<std::string>(number)
				);
	}
}

int ManagedFuzzyDataController::getMultiTSCount(int classNo) {
	return fuzzyMultiDataLearnAlgorithm.getDataSet()
			.getMutiTSCount(AxiomLib::FuzzyDataSet::Reference, classNo);
}

void ManagedFuzzyDataController::getMultiTS(int classNo, int tsNo, 
											std::vector<std::vector<double> > &multiTS)
{
	AxiomLib::FuzzyDataSet &dataSet = fuzzyMultiDataLearnAlgorithm.getDataSet();
	
	qDebug()<<"Performing reset...";
	AxiomLib::IntervalSet intervals = AxiomLib::IntervalSet::reset(dataSet, AxiomLib::FuzzyDataSet::Reference);
	qDebug()<<"Reset done";
	int size1;
	std::vector<int> size2;
	
	if(classNo < 0) {
		dataSet.getNormalClassSize(size1, size2);
		
		multiTS.resize(size2[tsNo]);
		
		for(unsigned int i = 0; i < multiTS.size(); i++) {
			dataSet.getNormalTSFromClass(
					multiTS[i],
					tsNo,
					i
					);
		}
	} else {
		std::vector<std::vector<int> > size3;
		
		dataSet.getClassSize(size1, size2, size3);
		
		multiTS.resize(size3[classNo][tsNo]);
		
		for(int i = 0; i < size3[classNo][tsNo]; i++) {
			dataSet.getTSByIndexFromClass(
					multiTS[i],
					classNo,
					tsNo,
					i
					);
		}
	}
	
	qDebug()<<"Performing apply...";
	intervals.apply(dataSet, AxiomLib::FuzzyDataSet::Reference);
	qDebug()<<"Apply done.";
}

//void ManagedFuzzyDataController::getTS(int classNo, int tsNo, int dimensionNo, std::vector<double> &ts) {
//	const AxiomLib::FuzzyDataSet &dataSet = fuzzyMultiDataLearnAlgorithm.getDataSet();
	
//	if(classNo < 0) {		
//		dataSet.getNormalTSFromClass(
//				ts,
//				tsNo,
//				dimensionNo
//				);

//	} else {
//		dataSet.getTSByIndexFromClass(
//				ts,
//				classNo,
//				tsNo,
//				dimensionNo
//				);
//	}	
//}

AxiomLib::IntInterval ManagedFuzzyDataController::getClippingInterval(int classNo, int multiTSNo) const {
	return fuzzyMultiDataLearnAlgorithm.getDataSet().getClassClippingInterval(classNo, multiTSNo);
}

void ManagedFuzzyDataController::setClippingInterval(const AxiomLib::IntInterval &interval, int classNo, int multiTSNo) {
	fuzzyMultiDataLearnAlgorithm.getDataSet().setClassClippingInterval(interval, classNo, multiTSNo);
}

AxiomLib::IntInterval ManagedFuzzyDataController::getRange(int classNo, int multiTSNo) const {
	int size = fuzzyMultiDataLearnAlgorithm.getDataSet().getClassMultiTSLength(classNo, multiTSNo);
	qDebug()<<"Entering getRange() "<<0<<" "<<size-1;
	return AxiomLib::IntInterval(0, size - 1);
	qDebug()<<"Exiting getRange()";
}

std::string ManagedFuzzyDataController::ecFolder() const {
	std::string result;
	env.getMandatoryParamValue(result, "AxiomBaseDir");
	return result;
}

std::string ManagedFuzzyDataController::asFolder() const {
	std::string result;
	env.getMandatoryParamValue(result, "AxiomSetBaseDir");
	return result;	
}

int ManagedFuzzyDataController::getTotalAbnormalTrajNumber() const {
//	int numOfClasses;
//	std::vector<int> numOfMultiTS;
//	std::vector<std::vector<int> > numOfTS;
//	fuzzyMultiDataLearnAlgorithm.getDataSet().getClassSize(numOfClasses, numOfMultiTS, numOfTS);
	
	int result = 0;
	for(int i = 0; i < fuzzyMultiDataLearnAlgorithm.getDataSet().getClassCount(); ++i) {
		result += fuzzyMultiDataLearnAlgorithm.getDataSet().getMutiTSCount(AxiomLib::FuzzyDataSet::Reference, i);
	}
	
	return result;
}
