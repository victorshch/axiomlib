#include "TemplateRecognizer.h"
#include "CompareClassFactory.h"
#include "LabelingStrategyFactory.h"
#include "GoalStrategyFactory.h"

#include "RecognizerImplBase.h"

using namespace AxiomLib;

RecognizerImplBase::RecognizerImplBase()
{
}

/****************************************************************************
*                       ReducedRecognizerDistance::run
*
*       Description:    Конструктор копирования -- клонирует labelingStrategy
*       Parameters:     other
*       Returns:        -
*       Throws:         -
*       Author:         wictor
*       History:
*
****************************************************************************/
RecognizerImplBase::RecognizerImplBase(const RecognizerImplBase &other)
	: m_comments(other.m_comments),
	  m_outputToFile(other.m_outputToFile),
	  compareStatistic(other.compareStatistic),
	  goalStrategy(other.goalStrategy),
	  labelingStrategy(boost::shared_ptr<LabelingStrategy>(other.labelingStrategy->copy()))
{ }

void AxiomLib::RecognizerImplBase::setComments(const bool newValue)
{
	m_comments = newValue;
}

void AxiomLib::RecognizerImplBase::setOutputToFile(const bool newValue)
{
	m_outputToFile = newValue;
}

int AxiomLib::RecognizerImplBase::setParamsFromEnv(const AxiomLib::Environment &env)
{
	std::string compareClassName;
	if(env.getStringParamValue(compareClassName, "compareClass")) {
		throw AxiomLibException("RecognizerImplBase::setParamsFromEnv : compare class not specified");
	}
	
	CompareClassFactory factory;
	compareStatistic = boost::shared_ptr<CompareStatistic>(factory.create(compareClassName));
	compareStatistic->setParamsFromEnv(env);
	
	std::string labelingStrategyName;
	env.getMandatoryParamValue(labelingStrategyName, "LabelingStrategy");
	
	LabelingStrategyFactory labelingStrategyFactory;
	labelingStrategy = boost::shared_ptr<LabelingStrategy>
			(labelingStrategyFactory.create(labelingStrategyName));
	labelingStrategy->initFromEnv(env);
	
	std::string goalStrategyName;
	GoalStrategyFactory gsf;
	if (env.getStringParamValue(goalStrategyName, "goalClass") < 0) {
		throw AxiomLibException("RecognizerImplBase::initFromEnv : goal strategy is undefined.");
	}
	goalStrategy = boost::shared_ptr<GoalStrategy>(gsf.create(goalStrategyName));
	goalStrategy->setParamsFromEnv(env);
	
	return initFromEnv(env);
}

int AxiomLib::RecognizerImplBase::setNominalParamsFromEnv(const AxiomLib::Environment &env)
{
	return setParamsFromEnv(env);
}

int AxiomLib::RecognizerImplBase::setDataSet(const AxiomLib::DataSet &/*newDataSet*/)
{
	//заглушка
	return -1;
}

int AxiomLib::RecognizerImplBase::setParamsOfDataSet(std::vector<int> &/*newParams*/)
{
	//заглушка
	return -1;
}

signed int AxiomLib::RecognizerImplBase::run(AxiomLib::AxiomSetPop &/*pop*/)
{
	//заглушка
	return -1;
}

signed int AxiomLib::RecognizerImplBase::run(AxiomLib::AxiomSetPop &/*pop*/, std::vector<double> &/*precs*/)
{
	//заглушка
	return -1;
}

signed int AxiomLib::RecognizerImplBase::run(AxiomLib::AxiomSet &axiomSet, AxiomLib::DataSet &dataSet, 
											 std::vector<int> &params, int &resFirst, int &resSecond, 
											 const int asIndex)
{
	std::vector<double> stat;
	return run(axiomSet, dataSet, params, resFirst, resSecond, stat, asIndex);
}

signed int AxiomLib::RecognizerImplBase::run(AxiomSet& axiomSet, DataSet& extDataSet,
											 std::vector<int>& extParams, int& resFirst, int& resSecond, 
											 std::vector<double> &stat, const int /*asIndex*/)
{
	/*
	  1. Получить маркировки тестовых траекторий и статистику использования аксиом
	  2. Посчитать число ошибок и статистику
	*/
	
	std::vector<std::vector<int> > labelings;
	std::vector<bool> refStat;
	std::vector<bool> testStat;
	
	// 1. Получаем маркировки тестовых траекторий и статистику использования аксиом
	performLabeling(axiomSet, extDataSet, extParams, labelings, refStat, testStat);
	
	int nTests = labelings.size();
	
	resFirst = 0;
	resSecond = 0;
	
	int resShouldBe = 0;
	
	//int nDims = extParams.size();
	std::vector <int> corrLabeling; // Здесь будет лежать корректная маркировка текущего ряда
	
	// 2. Считаем число ошибок
	for (int i = 0; i < nTests; ++i) {
		double tmpFirst = 0, tmpSecond = 0; // Переменные для суммирования статистики
		int tmpShouldBe = 0;
		extDataSet.getTSByIndexFromTests (corrLabeling, i, -1);
		compareStatistic->getStatistic(labelings[i], corrLabeling, tmpFirst, tmpSecond, comments());
		resFirst += (int)tmpFirst;
		resSecond += (int)tmpSecond;
		coutAbnormalSegments(corrLabeling, tmpShouldBe);
		resShouldBe += tmpShouldBe;
	}
	
	// Подсчет статистики
	countStat(stat, resFirst, resSecond, resShouldBe, refStat, testStat);
			
	return 0;	
}

void RecognizerImplBase::performLabeling(AxiomSet &axiomSet, DataSet &dataSet, 
										 std::vector<int> &params, 
										 std::vector<std::vector<int> > &result, 
										 std::vector<bool> &refAxiomUsage, 
										 std::vector<bool> &testAxiomUsage) {
	std::vector<TrajectorySampleDistance> sampleDistances;
	
	computeDistances(axiomSet, dataSet, params, sampleDistances, refAxiomUsage, testAxiomUsage);
	
	result.resize(sampleDistances.size());
	
	for (std::vector<TrajectorySampleDistance>::size_type i = 0; i < sampleDistances.size(); ++i) {
		labelingStrategy->performLabeling(sampleDistances[i], result[i]);
	}
}

double RecognizerImplBase::learn(TemplateRecognizer &templateRecognizer)
{
	std::vector<TrajectorySampleDistance> sampleDistances;
	std::vector<bool> refAxiomUsage, testAxiomUsage;
	
	DataSet& dataSet = templateRecognizer.prepDataSet;
	AxiomSet& axiomSet = *(templateRecognizer.axiomSet);
	computeDistances(axiomSet, dataSet, templateRecognizer.params,
					 sampleDistances, refAxiomUsage, testAxiomUsage);
	
	int numClasses = templateRecognizer.prepDataSet.getNumberOfClasses();
	
	std::vector<std::vector<int> > rightLabelings(numClasses);
	
	for(int i = 0; i < numClasses; ++i) {
		dataSet.getTSByIndexFromTests(rightLabelings[i], i, -1);
	}
	
	return labelingStrategy->train(sampleDistances, rightLabelings, 
								   compareStatistic.get(), goalStrategy.get());
	
}

/****************************************************************************
*				RecognizerImplBase::coutAbnormalSegments
*
*	Description:	Анализ одного теста - возвращает общее число траекторий нештатного поведения в данном тесте
*	Parameters:		labeling - корректная разметка теста
*					abnormalSegmentsNum - число участков нештатного поведения
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int RecognizerImplBase::coutAbnormalSegments(const std::vector<int>& labeling, int &abnormalSegmentsNum)
{
	unsigned int i = 0;
	unsigned int j = 0;
	abnormalSegmentsNum = 0;
	while (i < labeling.size()) {
		if (labeling[i] != 0) {
			abnormalSegmentsNum++;
			j = i+1;
			while (j < labeling.size()) {
				if (labeling[j] != labeling[i])
					break;
				j++;
			}
			i = j;
		} else {
			i++;
		}
	}
	return 0;	
}

/****************************************************************************
*					RecognizerImplBase::countStat
*
*	Description:	Вспомогательная процедура для подсчета статистики по результатам распознавания
*	Parameters:		stat - заполняемый вектор статистики
*					resFisrt - число ошибок первого рода
*					resSecond - число ошибок второго рода
*					abnormalSegmentsNum - общее число нештатных ситуаций, которые есть в тестовых данных
*					refStat - статистика по использованию аксиом для разметки эталонных траекторий
*					testStat - статистика по использованию аксиом для разметки тестовых рядов
*	Returns:		-
*	Throws:			AxiomLibException - если не входные параметры не согласованы
*	Author:			dk
*	History:
*
****************************************************************************/
void RecognizerImplBase::countStat(std::vector<double> &stat, int resFirst, int resSecond, 
								   int abnormalSegmentsNum, 
								   const std::vector<bool> &refStat, 
								   const std::vector<bool> &testStat)
{
	if (refStat.size() != testStat.size())
		throw AxiomLibException("RecognizerExtend::countStat : internal error - statistial data is uncoordinated.");
	stat.resize (refStat.size());
	int temp;
	double multiplier;
	multiplier = 0.5 * (double) (abnormalSegmentsNum - resSecond) / (double) (abnormalSegmentsNum + resFirst); // resFisrt и resSecond изначально перепутаны во всей программе, поэтому и здесь формула с такими подстановками
	for (unsigned int i = 0; i < refStat.size(); i++) {
		temp = 0;
		if (refStat[i]) temp++;
		if (testStat[i]) temp++;
		stat[i] = (double) temp * multiplier;
	}
}

bool AxiomLib::RecognizerImplBase::comments()
{
	return m_comments;
}

bool AxiomLib::RecognizerImplBase::outputToFile()
{
	return m_outputToFile;
}
