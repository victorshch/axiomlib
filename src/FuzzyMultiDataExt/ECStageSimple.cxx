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

#include "ECStageSimple.h"

#define eps						0.0000001 // ������������ � ��������� ��������, ����� ��������� ������ ��������� ����������
#define max_goal_val			10000 // ������������ �������� ������� ������� ��� ������������� ������� (������ ���������� ������� �����)

#define replacementProbability	0.5 // ����������� ��������� ������ �� ��������� ������� bestECs �����, ���� ���� ������ ��� ������ � �������� ��������� � ����� �� ��������� ������� �������
#define additionalECItems		10	// > 0 !!! ����� �������������� ������, �� ������� ����������� bestECs, ���� ���� ������ ��������

#undef AXIOMLIB_ECSTAGE_EXPERIMENTAL

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

ECStageSimple::ECStageSimple(FuzzyDataSet* fuzzyDataSet, 
										   ECTypeStage* stage0)
{

	this->stage0 = stage0;
	this->fuzzyDataSet = fuzzyDataSet;
	this->logger = Logger::getInstance();
	
	//TODO: �������� fuzzyDataSet � stage0 �� NULL
	
	//TODO: �������� �� ���������
	
	parent = stage0->getParent();

	goalOccurenceWeight = 0.0;

	entropyObjective = false;
	ecReversing = false;
	
	ecNameTemplate = "elemCondition";
}

ECSelection ecSelectionCreator(const ElemCondPlusStat& ec) { return ECSelection(ec); }

void ECStageSimple::setECs(const std::vector<std::vector<std::vector<std::vector<ElemCondPlusStat> > > > &value) {
	this->elemConditions.clear();
	TransformMultiVectorHelper<4>::transform(value, this->elemConditions, 
											 ecSelectionCreator
	);	
}

int ECStageSimple::initFromEnv(const Environment &env) {
	// ��������� ��������� ��������� ������������ �������
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

	env.getParamValue(entropyObjective, "ECEntropyObjective", false);

	env.getParamValue(ecReversing, "ECReversing", false);
	
	env.getStringParamValue(ecNameTemplate, "ECNameTemplate");
	return 0;
}

void ecSelector(ECSelection& ec) { ec.setSelected(true); }

void ECStageSimple::run() {
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
	
	// ����������������� �� ������� � ������������
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
	
	int totalCount = getECTotalCount();
	
	Logger::getInstance()->writeDebug("Finished EC selection stage. EC count: " + boost::lexical_cast<std::string>(totalCount));
}

const ElemCondPlusStat &ECStageSimple::getEC(int aType, int dimension, int type, int n) const {
	checkIndices(aType, dimension, type, n);
	return this->elemConditions[aType][dimension][type][n].element();
}

bool ECStageSimple::isECSelected(int aType, int dimension, int type, int n) const {
	checkIndices(aType, dimension, type, n);
	return this->elemConditions[aType][dimension][type][n].selected();
}

int sizeGetter(const std::vector<ECSelection>& v) { return (int)v.size(); }

void ECStageSimple::getECSize(std::vector<std::vector<std::vector<int> > > &result) const {

	TransformMultiVectorHelper<3>::transform(elemConditions, result, 
											 sizeGetter
	);
}

int ECStageSimple::getECSize() const {
	return elemConditions.size();
}

int ECStageSimple::getECSize(int aType) const {
	return elemConditions[aType].size();
}

int ECStageSimple::getECSize(int aType, int dimension) const {
	return elemConditions[aType][dimension].size();
}

int ECStageSimple::getECSize(int aType, int dimension, int ecType) const {
	return elemConditions[aType][dimension][ecType].size();
}

int ECStageSimple::getECTotalCount() const
{
	int result = 0;
	
	for(int i = 0; i  < getECSize(); ++i) {
		for(int j = 0; j < getECSize(i); ++j) {
			for(int k = 0; k  < getECSize(i, j); ++k) {
				result += getECSize(i, j, k);
			}
		}
	}
	
	return result;
}

void ECStageSimple::setECSelected(int aType, int dimension, int type, int n, bool value) {
	this->elemConditions[aType][dimension][type][n].setSelected(value);
}

void ECStageSimple::recalculateMatterECFunc(ElemCondPlusStat &ec, int abType) const {
	setTaskCount(1);
	this->matterECFunc(ec, ec.dimension, abType, 0);
}

void ECStageSimple::setTaskCount(int n) const {
	m_storage.resize(n);
}

void ECStageSimple::selectElemCond(int abnormalBehaviourType, int dimension, int dimensionIndex, int taskNo) {
	
//	// ��� �������� �� ������ �������� ����� �������, �.�. �� ������� �� ����� reserve()
//	this->elemConditions[abnormalBehaviourType].resize(
//			this->elemConditions[abnormalBehaviourType].size() + 1
//			);
	
	std::vector<std::vector<ECSelection> >& bestECs = this->elemConditions[abnormalBehaviourType][dimensionIndex];
	
	std::vector <ElemCondPlusStat> elemConds;
	
	int numTypes = stage0->getECTypeSize();
	
	elemConds.reserve(numTypes);
	
	// �������������� ����� ������������ ����� ��
	for(int i = 0; i < numTypes; i++) {
		if(stage0->isECTypeSelected(i)) {
			ElemCondPlus ec = stage0->getECType(i);
			if(!ecReversing || ec.sign == true) {
				ec.dimension = dimension;
				elemConds.push_back(ElemCondPlusStat(ec));
			}
		}
	}
	
	int elemCondsSize = elemConds.size();

	if(elemConds.size() == 0) {
		throw AxiomLibException("ECStage::selectElemCond() : no ECs selected for EC stage");
	}
	
	// ������� ����������� ��� �������� ���
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
	// ������ ������� ���� ��������� ����� ������� ��������� ���������� �� ������������ � ����������� ��������� ���
	// ���������� ������ ���������� � ������������ ��������. ������� ������� ������� ������ �� �����, ����� ���������� 1000.
	/*
	if (teachRow.size() > 1000) {
		teachRow.resize (1000);
	}*/
	
	// �������� ������ �� ����� ����� ������������ ������� - ��� ������� ���� ����� �������� ����� ������ ��������� �������
	bestECs.resize (elemCondsSize);

	// ������ � ����� ������� ������ ������������ �������
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
		// �������� ������ �� ����� ������ ������������ �������, ������� ��������� ��� ������� �� ����� �������
		bestECs[i].reserve(numBestECsMax);
		bestECs[i].clear();
		//bestECs[i].resize(numBestECs);
		// ������������� ������� �����
		elemConds[i].elemCondition->setLimits (leftLimit, rightLimit);
		if(elemConds[i].elemCondition->setLimits (teachRow, reserve, numOfLevels) == -1) {
			logger->writeComment("Could not select parameter grid for " + elemConds[i].ecTypeName());
			bestECs[i].resize(0);
			continue;
		}
		// ������ ������ �������� � ����� ����������
		elemConds[i].elemCondition->changeParams (0);
		// ��������� �������� ������� ������� ��� ������� � ��������� �����������
		matterECFunc (elemConds[i], dimension, abnormalBehaviourType, taskNo);
		ElemCondPlusStat currentBestEC(elemConds[i]);
		
		// ���������, ������� ����������, ��� �������� ������� ������� ������
		bool goalGrowing = true;
		
		// ���������� ��������� ���������� ������� �������� � ���������� ������ ��� ���� ��������� ���������� �������
		while (elemConds[i].elemCondition->changeParams(1) >= 0) {
			// ������� �������� ������� ������� ��� ���������� ����������
			matterECFunc (elemConds[i], dimension, abnormalBehaviourType, taskNo);
			//Logger::debug("Current goal: " + boost::lexical_cast<std::string>(elemConds[i].goal));
			// ����������� ���������� �� ��������� ������������ ������� � ������ ������� ����������
			if (abs(elemConds[i].goal - currentBestEC.goal) < eps) {
				// ������������ ������� ������ �������� ������� �������
				if(currentBestEC.goal < elemConds[i].goal) {
					currentBestEC = elemConds[i];
				}
			} else {
				// �������� ������� ������� ������������ ����������� �������� ����������
				if (elemConds[i].goal < currentBestEC.goal) {
					// �������� ������� ������� ����� - ����� ��������� ���� ���������� ������������ ������� �� ������������� ������� 
					if (goalGrowing) {
						// ������ �������� ������� ������� �� ���������� ������� ���������� ���� �� ��������� - ��������� ������� � ����� �� ��� ����������
						// ��������� �������, � ������� ����������� ��������� ��������, ����� ������
						storeBestECs (bestECs[i], currentBestEC, currentBestEC.goal, currentBestEC.statNormal, currentBestEC.statAbnorm);
						// ������� ��������� � ��������������� ���������� ��������� �� ��, ��� �������� ������� ������� ������ ������
						goalGrowing = false;
					}
				} else {
					// �������� ������� ������� ���������� - ��������� ���
					goalGrowing = true;
									}
				// ������������� ������� �������� �������� ������� � �������� ������� ��������� � ��������� ������� � ����� ��������� ������� �������
				currentBestEC = elemConds[i];
			}
		}
	}	
}

inline double binaryEntropy(double p) {
	double positiveEntropy = p > eps ? - p * (log(p)/log(2.0)) : 0;
	p = 1 - p;
	double negativeEntropy = p > eps ? - p * (log(p)/log(2.0)) : 0;

	return positiveEntropy + negativeEntropy;
}

inline double informationGainTerm(int normNegCount, int normPosCount, int abnNegCount, int abnPosCount) {
	double negFraction = (double) (normNegCount + abnNegCount) / (double) (normNegCount + abnNegCount + normPosCount + abnPosCount);
	double posFraction = 1.0 - negFraction;

	return - negFraction * binaryEntropy((double) normNegCount / (double)(normNegCount + abnNegCount))
			- posFraction * binaryEntropy((double)normPosCount / (double)(normPosCount + abnPosCount));
}

double ECStageSimple::matterECFunc (ElemCondPlusStat &ec, const int param, const int abnormalBehaviourType, int taskNo) const {
	// ����� ������� ������, ����� �� �������� ������ ��� ������
	std::vector <double>& curTS = m_storage[taskNo];
	int classCount = 0;
	int classLen = 0;

	int classMultiTSCount = fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Reference, abnormalBehaviourType);
	
	// ���� resize ����� ������� ��������� ������ ������ �����,
	// ����� ��� ��� �� ��������.
	// � ����������� ������� ��� ����� � ������ �������
	// ������ �� ���� ������ ��� ��������
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
	// ��������� ���������� �� ���������� ���������
	if (classLen > 0) {
		ec.statAbnorm = (double) classCount/ (double) classLen;
		ec.statOccurence = (double) abnOccCount / (double) classMultiTSCount;
	}
	else {
		ec.statAbnorm = -1.0; // - ������ ������ �� ����������
		ec.statOccurence = -1.0;
		std::cout << "\nWarning in  FuzzyMultiDataLearnAlgorithm::matterECFunc : incorrect dstaSet request for abnormal type.\n";
	}

	// ���� �� ����������� ����������� ���������
	//fuzzyDataSet->getNormalClassSize (numOfNormalMultiTS, numOfNormalTS);
	int normalMultiTSCount = fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Reference, -1);

	int totalNormalMultiTSCount = normalMultiTSCount;
	int normalOccCount = 0;

	int abnClassLen = classLen;
	int abnClassCount = classCount;

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

	int normClassCount = classCount;
	int normClassLen = classLen;

	// ��������� ���������� �� ���������� ���������
	if (classLen > 0)
		ec.statNormal = (double) classCount/ (double) classLen;
	else {
		ec.statNormal = -1.0; // - ������ ������ �� ����������
		std::cout << "\nWarning in FuzzyMultiDatadLearnAlgorithm::matterECFunc: incorrect dstaSet request.\n";
	}

	if(ecReversing) {
		if(ec.statOccurence < (double)normalOccCount / (double)totalNormalMultiTSCount
				|| ec.statAbnorm < ec.statNormal) {
			ec.statAbnorm = 1 - ec.statAbnorm;
			ec.statNormal = 1 - ec.statNormal;
			ec.statOccurence = 1 - ec.statOccurence;
			ec.sign = !ec.sign;
		}
	}
	if(!entropyObjective) {
		// ����������� �������� ������� �������
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
	} else {
		double igByPoints = informationGainTerm(normClassLen - normClassCount, normClassCount, abnClassLen - abnClassCount, abnClassCount);
		double igByTrajectories = informationGainTerm(totalNormalMultiTSCount - normalOccCount, normalOccCount, classMultiTSCount - abnOccCount, abnOccCount);
		ec.goal = 100.0 * (igByPoints + goalOccurenceWeight * igByTrajectories);
	}
	return ec.goal;
}

/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::numOfCarriedOutItems
*
*	Description:	������� ����� ����� � ����, �� ������� ������������ ������� �����������
*	Parameters:		ec - ����������� ������������ �������
*					row - ��� ��� �������� ������������� �������
*	Returns:		int - ����� �����, �� ������� ������������ ������� ���������
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int ECStageSimple::numOfCarriedOutItems (ElemCondPlus &ec, std::vector <double> &row) const {
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
*	Description:	������� ���������� ������ ������������ ������� � ��������������� 
*					�������� ������� �������
*	Parameters:		ec - ����������� ������������ �������
*					bestECs - ������ ������ ������� ���� �� ����
*	Returns:		1 - ���� ������� ���������
*					0 - ���� ������� �� ������ � ������ ������ 
*	Throws:			AxiomLibException - ���� ������� ������� �� �����������
*	Author:			wictor
*	History:
*
****************************************************************************/
inline int ECStageSimple::storeBestECs (std::vector <ECSelection> &bestECs, ElemCondPlusStat &ec, double &goal, double &statNorm, double &statAbnorm) const {
	//todo ��������; ���������, ����� ����� �������� �� ���������
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
			// ���� �������� ������� ������� ����� ������������� �� ������, �� � ������������ replacementProbability
			// ���������� �� ����� �� � ����������� ������� ��������
			if (((double) rand() / (double) RAND_MAX) < replacementProbability) {
				pair.first->element() = ec;
				return 1;
			} else {
				return 0;
			}			
		} else if (pair.first->element().goal < goal) {
			// ���� �������� ������� ������� ������ ������������ �� ������, 
			// �� ��������� ��, ������� �� � ����������� ���������
			pair.first->element() = ec;
			return 1;
		} else {
			// ����� -- �� ��������� ��
			return 0;
		}
	}
//	// �������� - ������ �� ������ �� � ������ ������ �� �������
//	double minVal = goal;
//	int whereToSave = -1;
//	double bestGoal = -1;
//	for (int i = 0; i < (int) bestECs.size(); i++) {
//		// ���� ���� ��������� ����, �� ������ ���������� � ����
//		if (bestECs[i].element().elemCondition == NULL) {
//			bestECs[i] = ec;
//			return 1;
//		} else {
//			// ���������� ������������ �������� ������� ������� ����� bestECs
//			if (bestGoal < bestECs[i].element().goal) 
//				bestGoal = bestECs[i].element().goal;
//			// ���������� - ���� ���������� ������ ������� ec
//			if (minVal > bestECs[i].element().goal) {
//				minVal = bestECs[i].element().goal;
//				whereToSave = i;
//			}
//		}
//	}
//	// ���� ������� ����� ������ - ��������� ���
//	if (whereToSave >= 0) {
//		// ��������� ������������ ������� ������� ����� ������ ������� � ������
//		bestECs[whereToSave] = ec;
//		return 1;
//	}
//	// ���� ������� �� ����� ����� ������, �� ����� ��� ���� ��� - �� ������� ��� �����
//	if (abs(bestGoal - goal) < eps) {
//		int curSize = (int) bestECs.size();
//		if (curSize < numBestECsMax) {
//			// ������ ���� ��� ����������� ��������� ������ bestECs
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
//			// ������ ������ bestECs ��� ������ �� ������� 

//			// ��� ������� - ����������, ������� �������� �������� - ��������� �� ������ ������������ �������
//			if (((double) rand() / (double) RAND_MAX) < replacementProbability) {
//				// �������� �������, ������� ���������
//				whereToSave = round ( (((double) std::rand() + 1.0)/((double) RAND_MAX + 2.0)) * ((double) curSize) - 0.5);
//				bestECs[whereToSave] = ec;
//				return 1;
//			}
//			return 0;
//		}
//	}
//	return 0;
}

void ECStageSimple::checkIndices(int i, int j, int k, int l) const {
	if(i < 0 || i >= this->elemConditions.size() || j < 0 || j >= elemConditions[i].size()
			|| k < 0 || k >= elemConditions[i][j].size() || l < 0 || l >= elemConditions[i][j][k].size()) {
		throw AxiomLibException("Invalid subscript in FuzzyMultiDataExt_EC : "+str(i)+" "+str(j)+" "+str(k)+" "+str(l));
	}
}

void ECStageSimple::setNames() {
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

std::string ECStageSimple::getECName(int abType, int dimension, int ecType, int n) const {
	char buffer[128];
	sprintf(buffer, "%s_at_%d_dim_%d_%d_%d", ecNameTemplate.c_str(), abType+1, dimension+1, ecType+1, n+1);
	return std::string(buffer);
}

bool isEcNull(const ECSelection& ec) { return ec.element().elemCondition == NULL; }

void AxiomLib::FuzzyMultiDataExt::ECStageSimple::prune() {
	for(int abType = 0; abType < getECSize(); abType++) {
		for(int dimension = 0; dimension < getECSize(abType); dimension++) {
			for(int ecType = 0; ecType < getECSize(abType, dimension); ecType++) {
				auto it = std::remove_if(elemConditions[abType][dimension][ecType].begin(), elemConditions[abType][dimension][ecType].end(), isEcNull);
				elemConditions[abType][dimension][ecType].erase(it, elemConditions[abType][dimension][ecType].end());
}}}	
}