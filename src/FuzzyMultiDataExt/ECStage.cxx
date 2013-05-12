#include <cstdlib>
#include <algorithm>
#include <cmath>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/minmax_element.hpp>

#include <omp.h>

#include "ECTypeStage.h"
#include "../Logger.h"
#include "Common.h"
#include "../FuzzyMultiDataExtAlgorithm.h"
//#include "../DistanceClusterizer.h"

#include "ECStage.h"

#define eps						0.0000001 // используется в некоторых формулах, чтобы исплючить ошибку машинного округления
#define max_goal_val			10000 // максимальное значение целевой функции для элементарного условия (просто достаточно большое число)

#define replacementProbability	0.5 // Вероятность замещения одного из элементов вектора bestECs новым, если этот вектор уже раздут и заполнен условиями с таким же значением целевой функции
#define additionalECItems		10	// > 0 !!! Число дополнительных слотов, на которое увеличивать bestECs, если этот вектор заполнен

#define AXIOMLIB_ECSTAGE_EXPERIMENTAL

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

ECStage::ECStage(FuzzyDataSet* fuzzyDataSet, 
										   ECTypeStage* stage0)
	: fuzzyDataSet(fuzzyDataSet), stage0(stage0), logger(Logger::getInstance())
{
	//TODO: проверка fuzzyDataSet и stage0 на NULL
	
	//TODO: значения по умлочанию
	
	parent = stage0->getParent();

	goalOccurenceWeight = 0.0;
	
	ecNameTemplate = "elemCondition";
}

ECSelection ecSelectionCreator(const ElemCondPlusStat& ec) { return ECSelection(ec); }

void ECStage::setECs(const std::vector<std::vector<std::vector<std::vector<ElemCondPlusStat> > > > &value) {
	this->elemConditions.clear();
	TransformMultiVectorHelper<4>::transform(value, this->elemConditions, 
											 ecSelectionCreator
	);	
}

int ECStage::initFromEnv(const Environment &env) {
	// Параметры алгоритма настройки элементарных условий
	if (env.getIntParamValue (leftLimit, "leftLimit") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : leftLimit is undefined.");
	if (env.getIntParamValue (rightLimit, "rightLimit") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : rightLimit is undefined.");
	if (env.getDoubleParamValue (reserve, "reserve") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : reserve is undefined.");
	if (env.getIntParamValue (numOfLevels, "numOfLevels") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : numOfLevels is undefined.");
	if (env.getIntParamValue (numBestECs, "numBestECs") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : numBestECs is undefined.");
	if (env.getIntParamValue (numBestECsMax, "numBestECsMax") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : numBestECsMax is undefined.");

	env.getParamValue(goalOccurenceWeight, "ECGoalOccurenceWeight");
	
	env.getStringParamValue(ecNameTemplate, "ECNameTemplate");
	return 0;
}

void ecSelector(ECSelection& ec) { ec.setSelected(true); }

void ECStage::run() {
	if(Logger::getInstance()->isWritingDebug()) {
		Logger::getInstance()->writeDebug("Starting EC selection stage");
		Logger::getInstance()->writeDebug("numOfLevels = "+boost::lexical_cast<std::string>(numOfLevels));
	}
	
	this->fuzzyDataSet->getParamNums(dataSetParams);
	dataSetParamNames = this->fuzzyDataSet->getParamNames();
		
	int classCount = fuzzyDataSet->getClassCount();
	int dimensionCount = dataSetParams.size();
	
	elemConditions.clear();
	elemConditions.resize(classCount);
	
	for(auto it = elemConditions.begin(); it != elemConditions.end(); ++it)
	{
		std::vector<std::vector<std::vector<ECSelection> > >& v = *it;
		v.resize(dimensionCount);
	}
	
	// Распараллеливание по классам и размерностям
	int taskCount = classCount * dimensionCount;
	
	setTaskCount(taskCount);
		
	#pragma omp parallel for schedule(dynamic, 1)
	for(int taskNo = 0; taskNo < taskCount; taskNo++) {
		int classIndex = taskNo / dimensionCount;
		int dimensionIndex = taskNo % dimensionCount;
		int dimension = dataSetParams[dimensionIndex];
		
		logger->writeComment("Abnormal type " + str(classIndex) + ", dimension " + dataSetParamNames[dimension]);
		selectElemCond(classIndex, dimension, dimensionIndex, taskNo);
		logger->writeComment("Finished abnormal type " + str(classIndex) + ", dimension " + dataSetParamNames[dimension]);
	}
	
	for(int abType = 0; abType < elemConditions.size(); abType++) {
		TransformMultiVectorHelper<3>::forEach(elemConditions[abType], 
											   ecSelector);
	}
	
	prune();
	
	setNames();
	
	Logger::getInstance()->writeDebug("Finished");
}

const ElemCondPlusStat &ECStage::getEC(int aType, int dimension, int type, int n) const {
	checkIndices(aType, dimension, type, n);
	return this->elemConditions[aType][dimension][type][n].element();
}

bool ECStage::isECSelected(int aType, int dimension, int type, int n) const {
	checkIndices(aType, dimension, type, n);
	return this->elemConditions[aType][dimension][type][n].selected();
}

int sizeGetter(const std::vector<ECSelection>& v) { return (int)v.size(); }

void ECStage::getECSize(std::vector<std::vector<std::vector<int> > > &result) const {

	TransformMultiVectorHelper<3>::transform(elemConditions, result, 
											 sizeGetter
	);
}

int ECStage::getECSize() const {
	return elemConditions.size();
}

int ECStage::getECSize(int aType) const {
	return elemConditions[aType].size();
}

int ECStage::getECSize(int aType, int dimension) const {
	return elemConditions[aType][dimension].size();
}

int ECStage::getECSize(int aType, int dimension, int ecType) const {
	return elemConditions[aType][dimension][ecType].size();
}

void ECStage::setECSelected(int aType, int dimension, int type, int n, bool value) {
	this->elemConditions[aType][dimension][type][n].setSelected(value);
}

void ECStage::recalculateMatterECFunc(ElemCondPlusStat &ec, int abType) const {
	setTaskCount(1);
	this->matterECFunc(ec, ec.dimension, abType, 0);
}

void ECStage::setTaskCount(int n) const {
	m_storage.resize(n);
}

void ECStage::selectElemCond(int abnormalBehaviourType, int dimension, int dimensionIndex, int taskNo) {
	
//	// Эта операция не должна занимать много времени, т.к. мы следали до этого reserve()
//	this->elemConditions[abnormalBehaviourType].resize(
//			this->elemConditions[abnormalBehaviourType].size() + 1
//			);
	
	std::vector<std::vector<ECSelection> >& bestECs = this->elemConditions[abnormalBehaviourType][dimensionIndex];
	
	std::vector <ElemCondPlusStat> elemConds;
	
	int numTypes = stage0->getECTypeSize();
	
	elemConds.reserve(numTypes);
	
	// Подготавливаем набор используемых типов ЭУ
	for(int i = 0; i < numTypes; i++) {
		if(stage0->isECTypeSelected(i)) {
			ElemCondPlus ec = stage0->getECType(i);
			ec.dimension = dimension;
			elemConds.push_back(ElemCondPlusStat(ec));
		}
	}
	
	int elemCondsSize = elemConds.size();
	
	// Достаем необходимый для обучения ряд
	std::vector<double> teachRow;
	int numOfMultiTS;
	std::vector <int> numOfTS;
	if(!parent->oneVsAllMode()) {
		fuzzyDataSet->getNormalClassSize (numOfMultiTS, numOfTS);
		if ((numOfTS.size() != numOfMultiTS) || (numOfMultiTS < 1))
			throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::selectElemCond : not enough data for the algorithm.");
		for (int i = 0; i < numOfMultiTS; i++) {
			if (numOfTS[i] > dimension) {
				fuzzyDataSet->getNormalTSFromClass (teachRow, i, dimension);
				if (teachRow.size() > 0)
					break;
			}
		}
	} else {
		if(fuzzyDataSet->getClassCount() <= 0) {
			throw AxiomLibException("FuzzyMultiDataExtAlgorithm::selectElemCond() : abnormal classes not found");
		}

		fuzzyDataSet->getTSByIndexFromClass(teachRow, 0, 0, dimension);
	}
	if (teachRow.size() < 1)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::selectElemCond : incorrect normal data set.");
	// Данный участок ряда необходим чтобы собрать начальную статистику по максимальным и минимальным значениям для
	// некоторого набора переменных в элементарных условиях. Поэтому слишком большие объемы не нужны, можно ограничить 1000.
	/*
	if (teachRow.size() > 1000) {
		teachRow.resize (1000);
	}*/
	
	// изменяем размер по числу типов элементраных условий - для каждого типа будет сохранет набор лучших вариантов условий
	bestECs.resize (elemCondsSize);

	// теперь в цикле обучаем каждое элементарное условие
	#pragma omp parallel for schedule(dynamic, 1)
	for (int i = 0; i < elemCondsSize; i++) {
		{
			int th_id = omp_get_thread_num();
			char buf[128];
			sprintf (buf, "\r\tAbType: %d\tDimension: %s\tElem condition: %d (%s) out of %d by thread %d.\t", 
					 abnormalBehaviourType, dataSetParamNames[dimension].c_str(), (i+1),
					 elemConds[i].ecTypeName().c_str(), (int)elemConds.size(), th_id);
			logger->writeComment(buf);
		}
		// Изменяем размер по числу лучших элементарных условий, которое сохранять для каждого из типов условий
		bestECs[i].reserve(numBestECsMax);
		bestECs[i].clear();
		//bestECs[i].resize(numBestECs);
		// Устанавливаем границы сетки
		elemConds[i].elemCondition->setLimits (leftLimit, rightLimit);
		if(elemConds[i].elemCondition->setLimits (teachRow, reserve, numOfLevels) == -1) {
			logger->writeComment("Could not select parameter grid for " + elemConds[i].ecTypeName());
			bestECs[i].resize(0);
			continue;
		}
		// Задаем первое значение в сетке параметров
		elemConds[i].elemCondition->changeParams (0);
		// Вычисляем значение целевой функции для условия с заданными параметрами
		matterECFunc (elemConds[i], dimension, abnormalBehaviourType, taskNo);
		ElemCondPlusStat currentBestEC(elemConds[i]);
		
		// Индикатор, который показывает, что значение целевой функции растет
		bool goalGrowing = true;
		
		// Итеративно повторяем вычисление целевой фунекции и сохранение лучших для всех вариантов параметров условия
		while (elemConds[i].elemCondition->changeParams(1) >= 0) {
			// Считаем значение целевой функции для измененных параметров
			matterECFunc (elemConds[i], dimension, abnormalBehaviourType, taskNo);
			//Logger::debug("Current goal: " + boost::lexical_cast<std::string>(elemConds[i].goal));
			// Определеяем необходимо ли сохранять элементарное условие с данным набором параметров
			if (abs(elemConds[i].goal - currentBestEC.goal) < eps) {
				// продолжается область одного значения целевой функции
				if(currentBestEC.goal < elemConds[i].goal) {
					currentBestEC = elemConds[i];
				}
			} else {
				// значение целевой функции относительно предыдущего значения изменилось
				if (elemConds[i].goal < currentBestEC.goal) {
					// Значение целевой функции упало - тогда сохраняем если необходимо элементарное условие из закончившейся области 
					if (goalGrowing) {
						// Значит значение целевой функции на предыдущей области параметров было на максимуме - сохраняем условие с одним их тех параметров
						// Сохраняем условие, в котором достигается локальный максимум, среди лучших
						storeBestECs (bestECs[i], currentBestEC, currentBestEC.goal, currentBestEC.statNormal, currentBestEC.statAbnorm);
						// Наконец сохраняем в соответствующей переменной указатель на то, что значение целевой функции начало падать
						goalGrowing = false;
					}
				} else {
					// Значение целевой функции возростает - сохраняем это
					goalGrowing = true;
									}
				// Устанавливаем текущее значение целеовой функции и обнуляем счетчик элементов в последней области с одним значением целевой функции
				currentBestEC = elemConds[i];
			}
		}
	}	
}

inline double binaryEntropy(double p) {
	double positiveEntropy = p > eps ? - p * log2(p) : 0;
	p = 1 - p;
	double negativeEntropy = p > eps ? - p * log2(p) : 0;

	return positiveEntropy + negativeEntropy;
}

double ECStage::matterECFunc (ElemCondPlusStat &ec, const int param, const int abnormalBehaviourType, int taskNo) const {
	// Берем готовый вектор, чтобы не выделять каждый раз память
	std::vector <double>& curTS = m_storage[taskNo];
	int classCount = 0;
	int classLen = 0;

	int classMultiTSCount = fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Reference, abnormalBehaviourType);
	
	// Этот resize будет взывать выделение памяти только тогда,
	// когда она еще не выделена.
	// В большинстве случаев при входе в данную функцию
	// памать на этот вектор уже выделена
	ec.statOccVector.resize(classMultiTSCount);
	
	int abnOccCount = 0;
	for (int j = 0; j < classMultiTSCount; j++) {
		curTS.clear();
		if (fuzzyDataSet->getTSByIndexFromClass (curTS, abnormalBehaviourType, j, param)) {
			int currentClassCount = numOfCarriedOutItems (ec, curTS);
			classCount += currentClassCount;
			classLen += curTS.size();
			if(currentClassCount > 0) {
				++abnOccCount;
				ec.statOccVector[j] = true;
			} else {
				ec.statOccVector[j] = false;
			}
		}
	}
	//std::cout<<"numOccurred = "<<numOccurred<<std::endl;
	// Заполняем статистику по нештатному поведению
	if (classLen > 0) {
		ec.statAbnorm = (double) classCount/ (double) classLen;
		ec.statOccurence = (double) abnOccCount / (double) classMultiTSCount;
	}
	else {
		ec.statAbnorm = -1.0; // - значит данные не определены
		ec.statOccurence = -1.0;
		std::cout << "\nWarning in  FuzzyMultiDataLearnAlgorithm::matterECFunc : incorrect dstaSet request for abnormal type.\n";
	}

	// Цикл по траекториям нормального поведения
	//fuzzyDataSet->getNormalClassSize (numOfNormalMultiTS, numOfNormalTS);
	int normalMultiTSCount = fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Reference, -1);

	int totalNormalMultiTSCount = normalMultiTSCount;
	int normalOccCount = 0;

	classCount = 0;
	classLen = 0;
	for (int j = 0; j < normalMultiTSCount; j++) {
		curTS.clear();
		if (fuzzyDataSet->getNormalTSFromClass (curTS, j, param)) {
			int currentClassCount = numOfCarriedOutItems (ec, curTS);
			classCount += currentClassCount;
			if(currentClassCount > 0) {
				++normalOccCount;
			}
			classLen += curTS.size();
		}
	}

	if(parent->oneVsAllMode()) {
		for(int classNo = 0; classNo < fuzzyDataSet->getClassCount(); ++classNo) {
			if(classNo == abnormalBehaviourType) {
				continue;
			}

			int classMultiTSCount = fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Reference, classNo);
			totalNormalMultiTSCount += classMultiTSCount;

			for (int j = 0; j < classMultiTSCount; j++) {
				curTS.clear();
				if (fuzzyDataSet->getTSByIndexFromClass (curTS, classNo, j, param)) {
					int currentClassCount = numOfCarriedOutItems (ec, curTS);
					classCount += currentClassCount;
					if(currentClassCount > 0) {
						++normalOccCount;
					}
					classLen += curTS.size();
				}
			}
		}
	}

	// Заполняем статистику по нештатному поведению
	if (classLen > 0)
		ec.statNormal = (double) classCount/ (double) classLen;
	else {
		ec.statNormal = -1.0; // - значит данные не определены
		std::cout << "\nWarning in FuzzyMultiDatadLearnAlgorithm::matterECFunc: incorrect dstaSet request.\n";
	}

#ifndef AXIOMLIB_ECSTAGE_EXPERIMENTAL
	// Определение значения целевой функции
	if (ec.statAbnorm < eps) {
		ec.goal = 0.0;
	} else {
		if (ec.statNormal < eps) {
			ec.goal = max_goal_val;
		} else {
			const double delta = 0.000005;
			double v1 = std::min ((double) ec.statAbnorm / (ec.statNormal + delta), (double)max_goal_val);
			double v2 = ec.statOccurence;
			ec.goal = v1 + goalOccurenceWeight * v2;
		}
	}
#else
//	Logger::debug("abnOccCount: " + boost::lexical_cast<std::string>(abnOccCount)
//				  + ", normOccCount: " + boost::lexical_cast<std::string>(normalOccCount));
	// Доля траекторий, которые ЭУ классифицировал как "1"
	double positiveFraction = (double)(abnOccCount + normalOccCount)
			/ (double)(classMultiTSCount + totalNormalMultiTSCount);

	// Доля траекторий, которые ЭУ классифицировал как "0"
	double negativeFraction = 1.0 - positiveFraction;

	double positiveAbnFraction = (double)abnOccCount / (double)(abnOccCount + normalOccCount);
	double positiveEntropy =  binaryEntropy(positiveAbnFraction);

	double negativeAbnFraction = (double)(classMultiTSCount - abnOccCount)
			/ (double)(classMultiTSCount + totalNormalMultiTSCount - abnOccCount - normalOccCount);
	double negativeEntropy = binaryEntropy(negativeAbnFraction);

	double ig = - positiveFraction * positiveEntropy - negativeFraction * negativeEntropy;

	// entropy
	ec.goal = 1000 * ig;

	if(ec.statOccurence < (double) normalOccCount / (double) totalNormalMultiTSCount) {
		ec.sign = !ec.sign;
		// TODO statistics
		//TODO occVector
	}
#endif
	return ec.goal;
}

/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::numOfCarriedOutItems
*
*	Description:	Подсчет числа точек в ряду, на которых элементарное условие выполняется
*	Parameters:		ec - проверяемое элементарное условие
*					row - ряд для порверки элементарного условия
*	Returns:		int - число точек, на которых элементарное условие выполнено
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int ECStage::numOfCarriedOutItems (ElemCondPlus &ec, std::vector <double> &row) const {
	int count = 0;
	for (unsigned long i = 0; i < row.size(); i++) {
		if (ec.check (i, row) > 0)
			count++;
	}
	return count;
}


bool goalCompare(const ECSelection& ecSel1, const ECSelection& ecSel2) { return ecSel1.element().goal < ecSel2.element().goal; }

/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::storeBestECs
*
*	Description:	Функция сохранения лучших элементарных условий и соответствующих 
*					значений целевой функции
*	Parameters:		ec - сохраняемое элементарное условие
*					bestECs - вектор лучших условий того же типа
*	Returns:		1 - если условие сохранено
*					0 - если условие не входит в список лучших 
*	Throws:			AxiomLibException - если входные вектора не согласованы
*	Author:			wictor
*	History:
*
****************************************************************************/
inline int ECStage::storeBestECs (std::vector <ECSelection> &bestECs, ElemCondPlusStat &ec, double &goal, double &statNorm, double &statAbnorm) const {
	//todo заглушка; исправить, чтобы таких ситуаций не возникало
	if(ec.elemCondition == NULL /*|| ec.goal <= -1*/) {
		Logger::debug("ec.elemCondition == NULL");
		return 0;
	}
	if(bestECs.size() < numBestECsMax) {
		//Logger::debug("Storing best EC");
		bestECs.push_back(ec);
		return 1;
	} else {
		auto pair = boost::minmax_element(bestECs.begin(), bestECs.end(), 
										  goalCompare);
		if(std::abs(goal - pair.second->element().goal) < eps) {
			// Если значение целевой функции равно максимальному из лучших, то с вероятностью replacementProbability
			// записываем на место ЭУ с минимальной целевой функцией
			if (((double) rand() / (double) RAND_MAX) < replacementProbability) {
				pair.first->element() = ec;
				return 1;
			} else {
				return 0;
			}			
		} else if (pair.first->element().goal < goal) {
			// Если значение целевой функции больше минимального из лучших, 
			// то добавляем ЭУ, заменяя ЭУ с минимальным значением
			pair.first->element() = ec;
			return 1;
		} else {
			// Иначе -- не добавляем ЭУ
			return 0;
		}
	}
//	// Проверка - входит ли данное ЭУ в список лучших за историю
//	double minVal = goal;
//	int whereToSave = -1;
//	double bestGoal = -1;
//	for (int i = 0; i < (int) bestECs.size(); i++) {
//		// Если есть свободный слот, то просто записываем в него
//		if (bestECs[i].element().elemCondition == NULL) {
//			bestECs[i] = ec;
//			return 1;
//		} else {
//			// Определяем максимальное значение целевой функции среди bestECs
//			if (bestGoal < bestECs[i].element().goal) 
//				bestGoal = bestECs[i].element().goal;
//			// Определяем - куда разместить данное условие ec
//			if (minVal > bestECs[i].element().goal) {
//				minVal = bestECs[i].element().goal;
//				whereToSave = i;
//			}
//		}
//	}
//	// Если условие среди лучших - сохраняем его
//	if (whereToSave >= 0) {
//		// Сохраняем элементарное условие заменяя самое слабое условие в наборе
//		bestECs[whereToSave] = ec;
//		return 1;
//	}
//	// Если условие по факту среди лучших, но места для него нет - то находим это место
//	if (abs(bestGoal - goal) < eps) {
//		int curSize = (int) bestECs.size();
//		if (curSize < numBestECsMax) {
//			// Значит есть еще возможность расширить вектор bestECs
//			std::vector <ElemCondPlusStat> tmpECs;
//			tmpECs.resize (curSize);
//			for (int i = 0; i < curSize; i++) {
//				tmpECs[i] = bestECs[i].element();
//				bestECs[i].element().clear();
//			}
//			bestECs.clear();
//			bestECs.resize (curSize + additionalECItems);
//			for (int i = 0; i < curSize; i++) {
//				bestECs[i].element() = tmpECs[i];
//				tmpECs[i].clear();
//			}
//			bestECs[curSize] = ec;
//			return 1;
//		} else {
//			// Значит вектор bestECs уже раздут до предела 

//			// Все условия - одинаковые, поэтому случайно выбираем - сохранять ли данное элементарное условие
//			if (((double) rand() / (double) RAND_MAX) < replacementProbability) {
//				// Выбираем элемент, который заместить
//				whereToSave = round ( (((double) std::rand() + 1.0)/((double) RAND_MAX + 2.0)) * ((double) curSize) - 0.5);
//				bestECs[whereToSave] = ec;
//				return 1;
//			}
//			return 0;
//		}
//	}
//	return 0;
}

void ECStage::checkIndices(int i, int j, int k, int l) const {
	if(i < 0 || i >= this->elemConditions.size() || j < 0 || j >= elemConditions[i].size()
			|| k < 0 || k >= elemConditions[i][j].size() || l < 0 || l >= elemConditions[i][j][k].size()) {
		throw AxiomLibException("Invalid subscript in FuzzyMultiDataExt_EC : "+str(i)+" "+str(j)+" "+str(k)+" "+str(l));
	}
}

void ECStage::setNames() {
	for(int aType = 0; aType < getECSize(); aType++) {
		for(int dimension = 0; dimension < getECSize(aType); dimension++) {
			for(int ecType = 0; ecType < getECSize(aType, dimension); ecType++) {
				for(int n = 0; n < getECSize(aType, dimension, ecType); n++) {
					this->elemConditions[aType][dimension][ecType][n].element().name = getECName(aType, dimension, ecType, n);
				}
			}
		}
	}
}

std::string ECStage::getECName(int abType, int dimension, int ecType, int n) const {
	char buffer[128];
	sprintf(buffer, "%s_at_%d_dim_%d_%d_%d", ecNameTemplate.c_str(), abType+1, dimension+1, ecType+1, n+1);
	return std::string(buffer);
}

bool isEcNull(const ECSelection& ec) { return ec.element().elemCondition == NULL; }

void AxiomLib::FuzzyMultiDataExt::ECStage::prune() {
	for(int abType = 0; abType < getECSize(); abType++) {
		for(int dimension = 0; dimension < getECSize(abType); dimension++) {
			for(int ecType = 0; ecType < getECSize(abType, dimension); ecType++) {
				auto it = std::remove_if(elemConditions[abType][dimension][ecType].begin(), elemConditions[abType][dimension][ecType].end(), isEcNull);
				elemConditions[abType][dimension][ecType].erase(it, elemConditions[abType][dimension][ecType].end());
}}}	
}
