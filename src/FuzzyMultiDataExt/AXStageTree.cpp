#include "AXStageTree.h"

#include <limits>
#include <algorithm>

#include "ECStage.h"

#include <boost/unordered_set.hpp>

#define eps 1e-7

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

AXTree::AXTree(const ElemCondPlus &ecPlus)
	: mEcPlus(ecPlus)
{
	mLeft = 0;
	mRight = 0;
}

AXTree::~AXTree()
{
	delete mLeft;
	delete mRight;
}

void AXTree::normalize()
{
	if(ecPlus().elemCondition == 0) {
		return;
	}

	bool alreadyNegated = false;

	if(right()) {
		if(right()->ecPlus().elemCondition == 0) {
			if(right()->ecPlus().sign == false) {
				mEcPlus.sign = !mEcPlus.sign;
				alreadyNegated = true;
			}
			delete right();
			setRight(0);
		} else {
			right()->normalize();
		}
	}

	if(left()) {
		if(left()->ecPlus().elemCondition == 0) {
			if(left()->ecPlus().sign == true && !alreadyNegated) {
				mEcPlus.sign = !mEcPlus.sign;
				alreadyNegated = true;
			}
			delete left();
			setLeft(0);
		} else {
			left()->normalize();
		}
	}
}

std::string axiomToString(const AxiomExpr& axiom) {
	std::string result = "(";
	for(int i = 0; i < axiom.expression.size(); ++i) {
		for(int j = 0; j  < axiom.expression[i].size(); ++j) {
			result += axiom.expression[i][j].ecTypeName();
			result += "&";
		}
		result += "|";
	}
	result += ")";
	return result;
}

AxiomExpr AXTree::toAxiom() const
{
	AxiomExpr result;

	if(right() && left()) {
		result = (AxiomExpr(ecPlus()) & right()->toAxiom()) | (AxiomExpr(!ecPlus()) & left()->toAxiom());
	} else if (right() && !left()) {
		result = (AxiomExpr(ecPlus()) & right()->toAxiom());
	} else if (!right() && left()) {
		result = AxiomExpr(ecPlus()) | left()->toAxiom();
	} else if (!right() && !left()) {
		result = AxiomExpr(ecPlus());
	} else {
		throw AxiomLibException("AXTree::toAxiom() : this can't happen");
	}

	return result;
}

std::string ecToString(const ElemCondPlus& ec) {
	return ec.elemCondition ? ((ec.sign ? "" : "not ") + ec.elemCondition->name()) :
							  (ec.sign? "+" : "-");
}

std::string AXTree::toString() const
{
	return "(" + ecToString(ecPlus()) + " " + (left() ? left()->toString() : "") + " " + (right() ? right()->toString() : "") + ")";
}

AXStageTree::AXStageTree(FuzzyDataSet *fuzzyDataSet, ECStage *stage1)
	: AXStage(fuzzyDataSet, stage1)
{
}

void AXStageTree::initFromEnv(const Environment &env)
{
	env.getMandatoryParamValue(numOfAxioms, "AXStageNumOfAxioms");
	env.getMandatoryParamValue(ecFraction, "AXStageECFraction");
	env.getMandatoryParamValue(axiomDepth, "AXStageAxiomDepth");

	env.getParamValue(oneVsAll, "OneVsAllMode", false);
	env.getParamValue(maxSplitLevel, "AXStageMaxSplitLevel", 0.2);
	env.getParamValue(threshSplitLevel, "AXStageThreshSplitLevel", 0.1);
}

void AXStageTree::setAxioms(const std::vector<std::vector<AxiomExprPlus> > &initial)
{
	axioms = initial;
}

const AxiomExprPlus &AXStageTree::getAX(int aType, int n) const
{
	return axioms[aType][n];
}

bool AXStageTree::isAXSelected(int aType, int n) const
{
	return true;
	//TODO
}

void AXStageTree::setAXSelected(int aType, int n, bool value)
{
	//TODO
}

int AXStageTree::getAXSize() const
{
	return axioms.size();
}

int AXStageTree::getAXSize(int aType) const
{
	return axioms[aType].size();
}

void AXStageTree::recalculateMatterAxiomFunc(AxiomExprPlus &ax, int abType)
{
	//TODO
}

void AXStageTree::run()
{
	int classCount = fuzzyDataSet->getClassCount();

	if(classCount != stage1->getECSize()) {
		throw AxiomLibException("AXStageTree::run() : inconsistent class count");
	}

	elemConds.resize(classCount);
	for(int i = 0; i < elemConds.size(); ++i) {
		for(int j = 0; j < stage1->getECSize(i); ++j) {
			for(int k = 0; k < stage1->getECSize(i, j); ++k) {
				for(int l = 0; l < stage1->getECSize(i, j, k); ++l) {
					if(stage1->isECSelected(i, j, k, l)) {
						elemConds[i].push_back(stage1->getEC(i, j, k, l));
					}
				}
			}
		}
	}

	axioms.resize(classCount);

	for(int i = 0; i < classCount; ++i) {
		Logger::comment("Creating axioms for class " + boost::lexical_cast<std::string>(i));
		formAxiomsForClass(i);
	}
}

AXTree *AXStageTree::constructTree(BootstrappedDataSet dataSet, int depth) const
{
	Logger::debug("AXStageTree::constructTree() : depth="+boost::lexical_cast<std::string>(depth));
	//Logger::debug("Current dataset: " + dataSet.toString());
	if(dataSet.normalTrajCount() == 0 || dataSet.abnormalTrajCount() == 0) {
		//Logger::debug("AXStageTree::constructTree() : one of classes is empty");
		ElemCondPlus result;
		result.elemCondition = 0;
		result.sign = dataSet.normalTrajCount() == 0;
		return new AXTree(result);
	}

	if(depth == 0) {
		//Logger::debug("AXStageTree::constructTree() : depth = 0");
		ElemCondPlus result;
		result.elemCondition = 0;
		//result.sign = dataSet.normalTrajCount() < dataSet.abnormalTrajCount();
		result.sign = dataSet.normalTrajFraction() < dataSet.abnormalTrajFraction();
		return new AXTree(result);
	}

	////Logger::debug("AXStageTree::constructTree() choosing EC");
	double bestIG;
	ElemCondPlus ec = chooseEC(dataSet, bestIG);

	AXTree* result = new AXTree(ec);

	Logger::debug("AXStageTree::constructTree() splitting dataset "  + dataSet.toString());
	double splitLevel;
	SplitDataSet split = dataSet.splitByEC(result->ecPlus(), splitLevel, maxSplitLevel);
	Logger::debug("Resulting datasets: " + split.left.toString() + ", " + split.right.toString());
	Logger::debug("Split level : " + boost::lexical_cast<std::string>(splitLevel));

//	if(split.right.totalTrajCount() == 0) {
//		Logger::debug("Couldn' t split dataset; trying ec negation");
//		result->ecPlus().sign = !result->ecPlus().sign;
//		split = dataSet.splitByEC(result->ecPlus(), splitLevel);
//		Logger::debug("Resulting datasets: " + split.left.toString() + ", " + split.right.toString());
//		Logger::debug("Split level : " + boost::lexical_cast<std::string>(splitLevel));
//	}

	if(split.left.totalTrajCount() == 0 || split.right.totalTrajCount() == 0) {
		delete result;
		Logger::debug("Couldn' t split dataset");
		ElemCondPlus result2;
		result2.elemCondition = 0;
		//result.sign = dataSet.normalTrajCount() < dataSet.abnormalTrajCount();
		result2.sign = dataSet.normalTrajFraction() < dataSet.abnormalTrajFraction();
		return new AXTree(result2);
	}


	//Logger::debug("AXStageTree::constructTree() recursing to depth " + boost::lexical_cast<std::string>(depth - 1));
	result->setLeft(constructTree(split.left, depth - 1));
	result->setRight(constructTree(split.right, depth - 1));

	return result;
}

ElemCondPlus AXStageTree::chooseEC(const BootstrappedDataSet &dataSet, double &bestIG) const
{
	int totalECCount = elemConds[dataSet.classNo()].size();
	int m = int(ecFraction * totalECCount);

	int ecCount = 0;
	boost::unordered_set<int> ecSet;

	bestIG = std::numeric_limits<double>::min();
	double bestSplitLevel = 1;
	int bestECIndex = 0;

	//TODO завершаемость!
	while(ecCount < m) {
		int currentIndex = rand() % totalECCount;

		if(ecSet.count(currentIndex)) continue;

		double splitLevel;
		double currentIG = informationGain(elemConds[dataSet.classNo()][currentIndex], dataSet, splitLevel);

		ecSet.insert(currentIndex);

		++ecCount;

		if(splitLevel > threshSplitLevel) {
			// В этом случае оптимизируем сначала splitLevel, потом ig
			if(splitLevel <= bestSplitLevel + eps) {
				if(splitLevel < bestSplitLevel - eps) {
					bestSplitLevel = splitLevel;
					bestIG = currentIG;
					bestECIndex = currentIndex;
				} else {
					if(currentIG > bestIG) {
						bestIG = currentIG;
						bestECIndex = currentIndex;
					}
				}
			}
		} else {
			// В этом случае оптимизируем только ig
			if(currentIG > bestIG) {
				bestIG = currentIG;
				bestECIndex = currentIndex;
				if(splitLevel < bestSplitLevel) {
					bestSplitLevel = splitLevel;
				}
			}
		}
	}

	return elemConds[dataSet.classNo()][bestECIndex];
}

double AXStageTree::informationGain(const ElemCondPlus &ec, const BootstrappedDataSet &dataSet, double& splitLevel) const
{
	SplitDataSet split = dataSet.splitByEC(ec, splitLevel, maxSplitLevel);

	return dataSet.entropy()
			- (double)split.left.totalTrajCount() / (double)(dataSet.totalTrajCount()) * split.left.entropy()
			- (double)split.right.totalTrajCount() / (double)(dataSet.totalTrajCount()) * split.right.entropy();
}


void AXStageTree::formAxiomsForClass(int classNo)
{
	axioms[classNo].resize(numOfAxioms);

	for(int i = 0; i < numOfAxioms; ++i) {
		Logger::debug("Bootstrapping data set...");
		BootstrappedDataSet currentDataSet(fuzzyDataSet, classNo, oneVsAll);
		if(currentDataSet.abnormalTrajCount() == 0 || currentDataSet.normalTrajCount() == 0) {
			Logger::debug("Failed to bootstrap dataset");
			continue;
		}
		Logger::debug("Creating axiom " + boost::lexical_cast<std::string>(i+1) + " out of "
					  + boost::lexical_cast<std::string>(numOfAxioms) + "...");

		Logger::debug("Constructing tree for axiom " + boost::lexical_cast<std::string>(i+1) + " out of "
					  + boost::lexical_cast<std::string>(numOfAxioms) + "...");
		AXTree* tree = constructTree(currentDataSet, axiomDepth);
		Logger::debug("Constructing axiom from tree " + boost::lexical_cast<std::string>(i+1) + " out of "
					  + boost::lexical_cast<std::string>(numOfAxioms) + "...");
		Logger::debug(tree->toString());
		tree->normalize();

		if(tree->ecPlus().elemCondition == 0) {
			Logger::debug("Failed to construct axiom tree");
			continue;
		}

		axioms[classNo][i] = AxiomExprPlus(tree->toAxiom());
		Logger::debug("Finished creating axiom " + boost::lexical_cast<std::string>(i+1) + " out of "
					  + boost::lexical_cast<std::string>(numOfAxioms) + ".");
		Logger::debug(axiomToString(axioms[classNo][i]));
		delete tree;
	}
}

inline double binaryEntropy(double p) {
	double positiveEntropy = p > eps ? - p * log2(p) : 0;

	p = 1 - p;

	double negativeEntropy = p > eps ? - p * log2(p) : 0;

	return positiveEntropy + negativeEntropy;
}

inline double informationGainTerm(int normNegCount, int normPosCount, int abnNegCount, int abnPosCount) {
	double negFraction = (double) (normNegCount + abnNegCount) / (double) (normNegCount + abnNegCount + normPosCount + abnPosCount);
	double posFraction = 1.0 - negFraction;

	return - negFraction * binaryEntropy((double) normNegCount / (double)(normNegCount + abnNegCount))
			- posFraction * binaryEntropy((double)normPosCount / (double)(normPosCount + abnPosCount));
}

double BootstrappedDataSet::entropy() const
{
	double abnFraction = (double) abnormalTrajCount() / (double) totalTrajCount();

	return binaryEntropy(abnFraction);
}

std::string BootstrappedDataSet::toString() const
{
	return "(" + boost::lexical_cast<std::string>(this->abnormalTrajCount()) + ", " + boost::lexical_cast<std::string>(this->normalTrajCount())
			+ ")";
}


BootstrappedDataSet::BootstrappedDataSet(const FuzzyDataSet *dataSet, int classNo, bool oneVsAll)
{
	mDataSet = dataSet;
	mClassNo = classNo;

	int totalTrajectoryCount = 0;
	int abnormalTrajectoryCount = 0;
	int normalTrajectoryCount = 0;
	for(int c = -1; c < mDataSet->getClassCount(); ++c) {
		int currentTrajCount = mDataSet->getMutiTSCount(FuzzyDataSet::Reference, c);
		if(c == mClassNo) {
			abnormalTrajectoryCount += currentTrajCount;
		} else if (oneVsAll || c == -1) {
			normalTrajectoryCount += currentTrajCount;
		}
	}

	totalTrajectoryCount = normalTrajectoryCount + abnormalTrajectoryCount;

	mTotalNormalCount = normalTrajectoryCount;
	mTotalAbnormalCount = abnormalTrajectoryCount;

	std::vector<std::pair<int, int> > indicesVector;
	indicesVector.reserve(totalTrajectoryCount);

	mNormalIndices.reserve(normalTrajectoryCount);
	mAbnormalIndices.reserve(abnormalTrajectoryCount);

	for(int traj = 0; traj < mDataSet->getMutiTSCount(FuzzyDataSet::Reference, -1); ++traj) {
		indicesVector.push_back(std::make_pair(-1, traj));
	}

	for(int traj = 0; traj < mDataSet->getMutiTSCount(FuzzyDataSet::Reference, classNo); ++traj) {
		indicesVector.push_back(std::make_pair(classNo, traj));
	}

	if(oneVsAll) {
		for(int c = 0; c < mDataSet->getClassCount(); ++c) {
			if(c == classNo) continue;
			for(int traj = 0; traj < mDataSet->getMutiTSCount(FuzzyDataSet::Reference, c); ++traj) {
				indicesVector.push_back(std::make_pair(c, traj));
			}
		}
	}

	for(int i = 0; i < indicesVector.size(); ++i) {
		int currentGlobalIndex = rand() % (int) indicesVector.size();
		//DEBUG ONLY!
		//int currentGlobalIndex = i;
		std::pair<int, int> currentIndex = indicesVector[currentGlobalIndex];
		if(currentIndex.first == mClassNo) {
			mAbnormalIndices.push_back(currentIndex);
		} else {
			mNormalIndices.push_back(currentIndex);
		}
	}
}

void BootstrappedDataSet::normalTraj(int i, int dimension, std::vector<double> &result) const
{
	mDataSet->getTSByIndex(FuzzyDataSet::Reference, result, mNormalIndices[i].first, mNormalIndices[i].second, dimension);
}

void BootstrappedDataSet::abnormalTraj(int i, int dimension, std::vector<double> &result) const
{
	mDataSet->getTSByIndex(FuzzyDataSet::Reference, result, mAbnormalIndices[i].first, mAbnormalIndices[i].second, dimension);
}

SplitDataSet BootstrappedDataSet::splitByEC(const ElemCondPlus &ecPlus, double &splitLevel, double maxSplitLevel) const
{	
	BootstrappedDataSet negative(mDataSet, mClassNo);
	BootstrappedDataSet positive(mDataSet, mClassNo);

	negative.mNormalIndices.reserve(mNormalIndices.size());
	negative.mAbnormalIndices.reserve(mAbnormalIndices.size());
	positive.mNormalIndices.reserve(mNormalIndices.size());
	positive.mAbnormalIndices.reserve(mAbnormalIndices.size());

	negative.mTotalAbnormalCount = mTotalAbnormalCount;
	negative.mTotalNormalCount = mTotalNormalCount;
	positive.mTotalAbnormalCount = mTotalAbnormalCount;
	positive.mTotalNormalCount = mTotalNormalCount;

	if(normalTrajCount() == 0 || abnormalTrajCount() == 0) {
		Logger::debug("BootstrappedDataSet::splitByEC() : nothing to split");
		if(normalTrajCount() == 0) {
			return SplitDataSet(negative, *this);
		} else {
			return SplitDataSet(*this, positive);
		}
	}

	std::vector<double> normalFreq(mNormalIndices.size());
	std::vector<double> abnormalFreq(mAbnormalIndices.size());

	int dimension = ecPlus.dimension;

	std::vector<double> traj;

	for(int i = 0; i < normalTrajCount(); ++i) {
		normalTraj(i, dimension, traj);
		unsigned long satCount = 0;
		for(unsigned long t = 0; t < traj.size(); ++t) {
			if(ecPlus.check(t, traj) == 1) {
				++satCount;
			}
		}
		normalFreq[i] = (double) satCount / (double) traj.size();
	}

	for(int i = 0; i < abnormalTrajCount(); ++i) {
		abnormalTraj(i, dimension, traj);
		unsigned long satCount = 0;
		for(unsigned long t = 0; t < traj.size(); ++t) {
			if(ecPlus.check(t, traj) == 1) {
				++satCount;
			}
		}
		abnormalFreq[i] = (double) satCount / (double) traj.size();
	}

	splitLevel = std::min(*std::min_element(normalFreq.begin(), normalFreq.end()),
						  *std::min_element(abnormalFreq.begin(), abnormalFreq.end()));

	if(splitLevel > maxSplitLevel) {
		splitLevel = maxSplitLevel;
	}

	for(int i = 0; i < normalTrajCount(); ++i) {
		if(normalFreq[i] <= splitLevel) {
			negative.mNormalIndices.push_back(mNormalIndices[i]);
		} else {
			positive.mNormalIndices.push_back(mNormalIndices[i]);
		}
	}

	for(int i = 0; i < abnormalTrajCount(); ++i) {
		if(abnormalFreq[i] <= splitLevel) {
			negative.mAbnormalIndices.push_back(mAbnormalIndices[i]);
		} else {
			positive.mAbnormalIndices.push_back(mAbnormalIndices[i]);
		}
	}

	return SplitDataSet(negative, positive);
}
