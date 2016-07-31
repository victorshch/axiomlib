#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "AxiomBase.h"

#include "FuzzyMultiDataExt/ECTypeStage.h"
#include "FuzzyMultiDataExt/ECStage.h"
#include "FuzzyMultiDataExt/ECStageSimple.h"
#include "FuzzyMultiDataExt/AXStage.h"
#include "FuzzyMultiDataExt/ASStage.h"
#include "FuzzyMultiDataExt/HeuristicsSet.h"

#include "ReducedRecognizerFactory.h"
#include "GoalStrategyFactory.h"

#include "FuzzyMultiDataExtAlgorithm.h"

#define DEFAULT_AX_STAGE std::string("simple")
#define DEFAULT_AS_STAGE std::string("simple")
#define DEFAULT_EC_STAGE std::string("simple")

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

FuzzyMultiDataExtAlgorithm::FuzzyMultiDataExtAlgorithm()
{	
	heuristicsSet = new HeuristicsSet();
	
	stage0 = new ECTypeStage(this);
	stage1 = 0;
	stage2 = 0;
	stage3 = 0;
	
	logger = Logger::getInstance();
}

FuzzyMultiDataExtAlgorithm::~FuzzyMultiDataExtAlgorithm() {
	delete stage3;
	delete stage2;
	delete stage1;
	delete stage0;
	delete heuristicsSet;
}

int FuzzyMultiDataExtAlgorithm::setComments(const bool newVal) {
	logger->setComments(newVal);
	
	return 0;
}

int FuzzyMultiDataExtAlgorithm::initFromEnv(const Environment &env) {
	// Инициализируем dataSet
	std::string datasetDir, datasetName;
	if (env.getStringParamValue(datasetDir, "BaseDataSetDir") < 0)
		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : data set directory is undefined.");
	if (env.getStringParamValue(datasetName, "DataSet") < 0)
		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : data set is undefined.");
	// считываем необходимые для данного класса параметры о наборе данных
	EnvDataSet envDataSet;
	envDataSet.readConfigFile (datasetDir, datasetName);
	// установка корректного обозначения NullStr - обозначение остутсвия в данной точке ряда какого либо нештатного поведения
	fuzzyDataSet.setNullStr (envDataSet);
	fuzzyDataSet.setNormalStr (envDataSet);
	// собственно считываем набор данных - заводим его во внутреннее представление
	fuzzyDataSet.readDataSet(datasetDir, datasetName);
	// восстанавливаем в данном классе вектор индексов параметров в каноническом представленнии по которым вести поиск нештатых ситуаций
	fuzzyDataSet.getParamNums(dataSetParams, env, envDataSet);

	// Определяем базовую директорию с наборами аксиом
	std::string curAxiomName;
	if (env.getStringParamValue(axiomBaseDir, "AxiomBaseDir") < 0)
		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : popPreset > 0 but axiom base dir is not set.");
	if (env.getStringParamValue(curAxiomName, "ECListFileName") >= 0) 
		fileNameECList.assign (curAxiomName);
	if (env.getStringParamValue(curAxiomName, "AxiomListFileName") >= 0) 
		fileNameAxiomList.assign (curAxiomName);

	// Сохранение файлов с описанием элементарных условий во внутрнеей переменной для последующего своевременного включения этого набора в рассмотрение
	env.getStringSetParamValue(predefinedECFileName, "ECFileName");

	// Сохранение файлов с описанием аксиом во внутрнеей переменной для последующего своевременного включения этого набора в рассмотрение
	env.getStringSetParamValue(predefinedAxiomsFileName, "AxiomFileName");

	// Параметры, по которым сохранять лучщие решения
	if (env.getStringParamValue(axiomSetBaseDir, "AxiomSetBaseDir") < 0)
		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : axiomSetBaseDir directory is undefined.");
	if (env.getStringParamValue(axiomSetName, "AxiomSetNameTemplate") < 0)
		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : axiomName to save is undefined.");

	if (env.getBoolParamValue(mOneVsAllMode, "oneVsAllMode") < 0) {
		// По умолчанию режим "один против всех" отключен
		mOneVsAllMode = false;
	}

	Logger::getInstance()->writeDebug("Initializing heuristics set");
	heuristicsSet->initFromEnv(env);
	
	std::string ecStageName;
	env.getParamValue(ecStageName, "ecStage", DEFAULT_EC_STAGE);

	stage1 = ECStage::create(ecStageName, &fuzzyDataSet, stage0);

	if(stage1 == 0) {
		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : invalid name of ec stage : '" + ecStageName + "'");
	}

	std::string axStageName;
	env.getParamValue(axStageName, "axiomStage", DEFAULT_AX_STAGE);

	stage2 = AXStage::create(axStageName, &fuzzyDataSet, stage1);
	if(stage2 == 0) {
		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : invalid name of axiom stage : '" + axStageName + "'");
	}

	std::string asStageName;
	env.getParamValue(asStageName, "axiomSetStage", DEFAULT_AS_STAGE);
	
	stage3 = ASStage::create(asStageName, &fuzzyDataSet, stage2);
	if(stage3 == 0) {
		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : invalid name of axiom set stage : '" + asStageName + "'");
	}
	
	stage0->initFromEnv(env);
	stage1->initFromEnv(env);
	stage2->initFromEnv(env);
	stage3->initFromEnv(env);
	
	return 0;	
}

int FuzzyMultiDataExtAlgorithm::initFromEnvRecognitionOnly(const Environment &env) {
	// Инициализируем dataSet
	std::string datasetDir, datasetName;
	if (env.getStringParamValue(datasetDir, "BaseDataSetDir") < 0)
		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : data set directory is undefined.");
	if (env.getStringParamValue(datasetName, "DataSet") < 0)
		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : data set is undefined.");
	// считываем необходимые для данного класса параметры о наборе данных
	EnvDataSet envDataSet;
	envDataSet.readConfigFile (datasetDir, datasetName);
	// установка корректного обозначения NullStr - обозначение остутсвия в данной точке ряда какого либо нештатного поведения
	fuzzyDataSet.setNullStr (envDataSet);
	fuzzyDataSet.setNormalStr (envDataSet);
	// собственно считываем набор данных - заводим его во внутреннее представление
	fuzzyDataSet.readDataSet(datasetDir, datasetName);
	// восстанавливаем в данном классе вектор индексов параметров в каноническом представленнии по которым вести поиск нештатых ситуаций
	fuzzyDataSet.getParamNums(dataSetParams, env, envDataSet);

	std::string asStageName;
	env.getMandatoryParamValue(asStageName, "axiomSetStage");

	stage3 = ASStage::create(asStageName, &fuzzyDataSet, stage2);
	if(stage3 == 0) {
		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : invalid name of axiom set stage : '" + asStageName + "'");
	}

	// Параметры, по которым сохранять лучщие решения
	if (env.getStringParamValue(axiomSetBaseDir, "AxiomSetBaseDir") < 0)
	{}
//		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : axiomSetBaseDir directory is undefined.");
	if (env.getStringParamValue(axiomSetName, "AxiomSetNameTemplate") < 0) {}
//		throw AxiomLibException("FuzzyMultiDataExtAlgorithm::setParamsFromEnv : axiomName to save is undefined.");


	stage3->initFromEnv(env);

	return 0;
}

FuzzyDataSet &FuzzyMultiDataExtAlgorithm::getDataSet() {
	return this->fuzzyDataSet;
}

const FuzzyDataSet &FuzzyMultiDataExtAlgorithm::getDataSet() const {
	return this->fuzzyDataSet;
}

void FuzzyMultiDataExtAlgorithm::setECTypes(const std::vector<ElemCondPlus> &ecTypes) {
	this->stage0->setECTypes(ecTypes);
}

const ElemCondPlus& FuzzyMultiDataExtAlgorithm::getECType(int n) const {
	return stage0->getECType(n);
}

bool FuzzyMultiDataExtAlgorithm::isECTypeSelected(int n) const {
	return stage0->isECTypeSelected(n);
}

void FuzzyMultiDataExtAlgorithm::setECTypeSelected(int n, bool value) {
	stage0->setECTypeSelected(n, value);
}

int FuzzyMultiDataExtAlgorithm::getECTypeSize() const {
	return stage0->getECTypeSize();
}

void FuzzyMultiDataExtAlgorithm::setECs(const std::vector<std::vector<std::vector<std::vector<ElemCondPlusStat> > > > &ecs) {
	this->stage1->setECs(ecs);
}

const ElemCondPlusStat& FuzzyMultiDataExtAlgorithm::getEC(int aType, int dimension, int type, int n) const {
	return stage1->getEC(aType, dimension, type, n);
}

const ElemCondPlusStat& FuzzyMultiDataExtAlgorithm::getEC(const ECIndex &index) const {
	return getEC(index.abType, index.dimension, index.ecType, index.n);
}

bool FuzzyMultiDataExtAlgorithm::isECSelected(int aType, int dimension, int type, int n) const {
	return stage1->isECSelected(aType, dimension, type, n);
}

bool FuzzyMultiDataExtAlgorithm::isECSelected(const ECIndex &index) {
	return isECSelected(index.abType, index.dimension, index.ecType, index.n);
}

void FuzzyMultiDataExtAlgorithm::setECSelected(int aType, int dimension, int type, int n, bool value) {
	stage1->setECSelected(aType, dimension, type, n, value);
}

void FuzzyMultiDataExtAlgorithm::setECSelected(const ECIndex &index, bool value) {
	setECSelected(index.abType, index.dimension, index.ecType, index.n, value);
}

FuzzyMultiDataExtAlgorithm::ECIndex::ECIndex(int abType, int dimension, int ecType, int n)
	: abType(abType),
      dimension(dimension),
      ecType(ecType),
      n(n) 
{ }

FuzzyMultiDataExtAlgorithm::ECIndex FuzzyMultiDataExtAlgorithm::makeECIndex(int abType, int dimension, int ecType, int n) {
	return ECIndex(abType, dimension, ecType, n);
}

void FuzzyMultiDataExtAlgorithm::getECSize(std::vector<std::vector<std::vector<int> > > &result) const {
	stage1->getECSize(result);
}

int FuzzyMultiDataExtAlgorithm::getECSize() const {
	return stage1->getECSize();
}

int FuzzyMultiDataExtAlgorithm::getECSize(int aType) const {
	return stage1->getECSize(aType);
}

int FuzzyMultiDataExtAlgorithm::getECSize(int aType, int dimension) const {
	return stage1->getECSize(aType, dimension);
}

int FuzzyMultiDataExtAlgorithm::getECSize(int aType, int dimension, int ecType) const {
	return stage1->getECSize(aType, dimension, ecType);
}

void FuzzyMultiDataExtAlgorithm::saveECs(const std::string &dir, const std::string &listFileName, bool selectedOnly) const {
	std::vector<std::vector<std::vector<std::vector<std::string> > > > ecNames;
	
	ecNames.resize(getECSize());
	
	for(int abType = 0; abType < ecNames.size(); abType++) {
		ecNames[abType].resize(getECSize(abType));
		for(int dimension = 0; dimension < ecNames[abType].size(); dimension++) {
			ecNames[abType][dimension].resize(getECSize(abType, dimension));
			for(int ecType = 0; ecType < ecNames[abType][dimension].size(); ecType++) {
				ecNames[abType][dimension][ecType].reserve(getECSize(abType, dimension, ecType));
				for(int n = 0; n < getECSize(abType, dimension, ecType); n++) {
					if(!selectedOnly || isECSelected(abType, dimension, ecType, n)) {
						ecNames[abType][dimension][ecType].push_back(getEC(abType, dimension, ecType, n).name);
						getEC(abType, dimension, ecType, n).saveECToFile(dir, ecNames[abType][dimension][ecType].back(), getDataSet().getParamNames());
					}
				}
			}
		}
	}
	
	AxiomBase base;
	base.saveToList(dir, listFileName, ecNames);
}

struct ecLoader
{
	ecLoader(const std::string& dir, const std::vector<std::string>& paramNames)
	{
		m_dir = dir;
		m_paramNames = paramNames;
	}

	ElemCondPlusStat operator()(const std::string& ecName)
	{
		ElemCondPlusStat result;
		result.initFromFile(m_dir, ecName, m_paramNames);
		return result;
	}

private:
	std::string m_dir;
	std::vector<std::string> m_paramNames;
};

struct axLoader
{
	axLoader(const std::string& dir, const std::vector<std::string>& paramNames)
	{
		m_dir = dir;
		m_paramNames = paramNames;
	}

	AxiomExprPlus operator()(const std::string& axName)
	{
		AxiomExprPlus result;
		result.initAxiomFromFile(m_dir, axName, m_paramNames);
		return result;
	}

private:
	std::string m_dir;
	std::vector<std::string> m_paramNames;
};

void FuzzyMultiDataExtAlgorithm::loadECs(const std::string &dir, const std::string &listFileName) {
	AxiomBase base;
	std::vector<std::vector<std::vector<std::vector<std::string> > > > ecNames;
	
	base.readFromList(dir + "/" + listFileName, ecNames);
	
	if(ecNames.size() != getDataSet().getClassCount()) {
		throw AxiomLibException("Abnormal class count in file "+listFileName+" doesn't match that of dataset");
	}
	
	int dimensionCount = (int)getDataSet().getParamNums().size();
	
	for(auto it = ecNames.begin(); it != ecNames.end(); ++it) {
		if(it->size() != dimensionCount) {
			throw AxiomLibException("Dimension count in file "+listFileName+" doesn't match that of dataset");
		}
	}
	
	std::vector<std::vector<std::vector<std::vector<ElemCondPlusStat> > > > ecs;
	
	TransformMultiVectorHelper<4>::transform(ecNames, ecs, ecLoader(dir, this->getDataSet().getParamNames()));
	
	setECs(ecs);
}

void FuzzyMultiDataExtAlgorithm::setAxioms(const std::vector<std::vector<AxiomExprPlus> > &axioms) {
	stage2->setAxioms(axioms);
}

const AxiomExprPlus& FuzzyMultiDataExtAlgorithm::getAX(int aType, int n) const {
	return stage2->getAX(aType, n);
}

bool FuzzyMultiDataExtAlgorithm::isAXSelected(int aType, int n) const {
	return stage2->isAXSelected(aType, n);
}

void FuzzyMultiDataExtAlgorithm::setAXSelected(int aType, int n, bool value) {
	stage2->setAXSelected(aType, n, value);
}

void FuzzyMultiDataExtAlgorithm::getAXSize(std::vector<int> &result) const {
	stage2->getAXSize(result);
}

int FuzzyMultiDataExtAlgorithm::getAXSize() const {
	return stage2->getAXSize();
}

int FuzzyMultiDataExtAlgorithm::getAXSize(int aType) const {
	return stage2->getAXSize(aType);
}

void FuzzyMultiDataExtAlgorithm::saveAXs(const std::string &dir, const std::string &listFileName, bool selectedOnly) const {
	std::vector<std::vector<std::string> > axNames;
	AxiomBase base;
	
	axNames.resize(getAXSize());
	
	for(int abType = 0; abType < axNames.size(); abType++) {
		axNames[abType].reserve(getAXSize(abType));
		for(int n = 0; n < getAXSize(abType); n++) {
			if(!selectedOnly || isAXSelected(abType, n)) {
				axNames[abType].push_back(getAX(abType, n).name());
				getAX(abType, n).saveAxiomToFile(dir, axNames[abType].back(), this->getDataSet().getParamNames());
			}
		}
	}
	
	base.saveToList(dir, listFileName, axNames);
}

void FuzzyMultiDataExtAlgorithm::loadAXs(const std::string &dir, const std::string &listFileName) {
	std::vector<std::vector<std::string> > axNames;
	AxiomBase base;

	base.readFromList(dir + "/" + listFileName, axNames);
	
	if(axNames.size() != getDataSet().getClassCount()) {
		throw AxiomLibException("Abnormal class count in file "+listFileName+" doesn't match that of dataset");
	}
	
	std::vector<std::vector<AxiomExprPlus> > axioms;
	
	TransformMultiVectorHelper<2>::transform(axNames, axioms, axLoader(dir, this->getDataSet().getParamNames()));
	
	setAxioms(axioms);
}

AxiomExprSetPlus& FuzzyMultiDataExtAlgorithm::getAS(int n) {
	return stage3->getAS(n);
}

const AxiomExprSetPlus& FuzzyMultiDataExtAlgorithm::getAS(int n) const {
	return stage3->getAS(n);
}

int FuzzyMultiDataExtAlgorithm::getASSize() const {
	return stage3->getASSize();
}

void FuzzyMultiDataExtAlgorithm::saveASs(const std::string &dir) const {
	for(int i = 0; i < getASSize(); i++) {
		getAS(i).saveAxiomSetToFile(dir, getAS(i).name(), this->getDataSet().getParamNames());
	}
}

void FuzzyMultiDataExtAlgorithm::saveASs() const {
	saveASs(this->axiomSetBaseDir);
}

void FuzzyMultiDataExtAlgorithm::loadASs(const std::string& dir, const std::vector<std::string> &asNames) {
	std::vector<AxiomExprSetPlus> axiomSets;
	axiomSets.reserve(asNames.size());

	for(auto it = asNames.begin(); it != asNames.end(); ++it) {
		AxiomExprSetPlus as;
		as.initAxiomSetFromFile(dir, *it, this->getDataSet().getParamNames());
		axiomSets.push_back(as);
	}
	
	setAxiomSets(axiomSets);
}

void FuzzyMultiDataExtAlgorithm::saveAxiomSetsToXml(const std::string &dirName) const
{
//	boost::filesystem::path dir(dirName);
	Logger::comment("Saving axoiom sets to directory '" + dirName + "'");
	for(int i = 0; i < getASSize(); ++i) {
		const AxiomExprSetPlus& aesp = getAS(i);
		//boost::filesystem::path currentPath = dir;
		std::string currentFileName = dirName + "/" + aesp.name() + ".xml";
		//currentPath /= aesp.name() + ".xml";
		Logger::comment("Saving axiom set to " + currentFileName);
		std::ofstream ofstr(currentFileName);
		if(ofstr.good())
		{
			boost::archive::xml_oarchive archive(ofstr);
			archive << boost::serialization::make_nvp("AxiomExprSetPlus", aesp);
		}
		else
		{
			throw AxiomLibException("ECStatistics:saveToFile(): Couldn't open file '"
									+ currentFileName + "' for writing");
		}

	}
}

void FuzzyMultiDataExtAlgorithm::saveAxiomSetsToXml() const
{
	saveAxiomSetsToXml(this->axiomSetBaseDir);
}

void FuzzyMultiDataExtAlgorithm::loadAxiomSetsFromXml(const std::string &dirName, const std::vector<std::string> &asNames)
{
//	boost::filesystem::path dir(dirName);
	std::vector<AxiomExprSetPlus> axiomSets;
	axiomSets.reserve(asNames.size());
	for(int i = 0; i < (int)asNames.size(); ++i) {
		std::string currentName = asNames[i];
		std::string currentFileName = dirName + "/" + currentName + ".xml";
		Logger::comment("Loading axiom set from '" + currentFileName + "'");
		boost::filesystem::path currentPath(currentFileName);
//		if(!boost::filesystem::exists(currentPath)) {
//			Logger::comment("Warning: specified axiom set file '" + currentPath.filename() + "' does not exist");
//			continue;
//		}
		std::ifstream ifstr(currentFileName);
		if(ifstr.good())
		{
			AxiomExprSetPlus aesp;
			boost::archive::xml_iarchive archive(ifstr);
			archive >> boost::serialization::make_nvp("AxiomExprSetPlus", aesp);
			axiomSets.push_back(aesp);
		}
		else
		{
			throw AxiomLibException("FuzzyMultiDataExtAlgorithm::loadAxiomSetsFromXml(): Couldn't open file '"
									+ currentFileName + "' for reading");
		}
	}

	setAxiomSets(axiomSets);
}

void FuzzyMultiDataExtAlgorithm::loadAxiomSetsFromXml(const std::vector<std::string> &asNames)
{
	loadAxiomSetsFromXml(this->axiomSetBaseDir, asNames);
}

void FuzzyMultiDataExtAlgorithm::recalculateMatterECFunc(ElemCondPlusStat &ec, int abType) {
	stage1->recalculateMatterECFunc(ec, abType);
}

void FuzzyMultiDataExtAlgorithm::recalculateMatterAxiomFunc(AxiomExprPlus &ax, int abType) {
	stage2->recalculateMatterAxiomFunc(ax, abType);
}

void FuzzyMultiDataExtAlgorithm::recalculateMatterAxiomSetFunc(AxiomExprSetPlus& as) {
	stage3->recalculateMatterASFunc(as);
}

void FuzzyMultiDataExtAlgorithm::run() {
    runZeroLevelHeuristics();
	runFirstLevel();
	runFirstLevelHeuristics();
	runSecondLevel();
	runSecondLevelHeuristics();
	runThirdLevel();
}

void FuzzyMultiDataExtAlgorithm::runZeroLevelHeuristics() {
	heuristicsSet->apply(this, 0);
}

void FuzzyMultiDataExtAlgorithm::runFirstLevel() {
	stage1->run();
}

void FuzzyMultiDataExtAlgorithm::runFirstLevelHeuristics() {
	heuristicsSet->apply(this, 1);
}

void FuzzyMultiDataExtAlgorithm::runSecondLevel() {
	stage2->run();
}

void FuzzyMultiDataExtAlgorithm::runSecondLevelHeuristics() {
	heuristicsSet->apply(this, 2);
}

void FuzzyMultiDataExtAlgorithm::runThirdLevel() {
	stage3->run();

	// This is bad programming, but i'm lazy
	int numLength = (int) boost::lexical_cast<std::string>(stage3->getASSize() - 1).size();

	for(int i = 0; i < stage3->getASSize(); ++i) {
		std::ostringstream ss;
		ss << std::setw(numLength) << std::setfill('0') << (i+1);
		stage3->getAS(i).setName(this->axiomSetName + ss.str());
	}
}

void FuzzyMultiDataExtAlgorithm::setAxiomSets(const std::vector<AxiomExprSetPlus> &axiomSets) {
	stage3->setAxiomSets(axiomSets);
}
