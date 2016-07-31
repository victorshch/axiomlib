#ifndef AXSTAGETREE_H
#define AXSTAGETREE_H

#include "AXStage.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

// a decision tree corresponding to an axiom
class AXTree {
public:
	AXTree(const ElemCondPlus& ecPlus);
	~AXTree();

	// takes ownership
	void setLeft(AXTree* value) { mLeft = value; }
	void setRight(AXTree* value) { mRight = value; }

	AXTree* left() const { return mLeft; }
	AXTree* right() const { return mRight; }

	const ElemCondPlus& ecPlus() const { return mEcPlus; }
	ElemCondPlus& ecPlus() { return mEcPlus; }

	// Negate ECs where appropriate and remove nodes with ElemCondPlus.elemCondition = 0
	void normalize();

	AxiomExpr toAxiom() const;

	std::string toString() const;
private:
	AXTree* mLeft;
	AXTree* mRight;

	ElemCondPlus mEcPlus;
};

class SplitDataSet;

// bootstrapped view of the dataset
class BootstrappedDataSet {
public:
	BootstrappedDataSet(const FuzzyDataSet* dataSet, int classNo, bool oneVsAll);

	int normalTrajCount() const { return mNormalIndices.size(); }

	double normalTrajFraction() const { return (double)mTotalNormalCount / (double) normalTrajCount(); }

	void normalTraj(int i, int dimension, std::vector<double>& result) const;

	int abnormalTrajCount() const { return mAbnormalIndices.size(); }

	double abnormalTrajFraction() const { return (double) mTotalAbnormalCount / (double) abnormalTrajCount(); }

	void abnormalTraj(int i, int dimension, std::vector<double>& result) const;

	int totalTrajCount() const { return normalTrajCount() + abnormalTrajCount(); }

	int classNo() const { return mClassNo; }

	SplitDataSet splitByEC(const ElemCondPlus& ecPlus, double& splitLevel, double maxSplitLevel) const;

	double entropy() const;

	std::string toString() const;
private:
	BootstrappedDataSet(const FuzzyDataSet* dataSet, int classNo)
		: mDataSet(dataSet), mClassNo(classNo) {}

	const FuzzyDataSet* mDataSet;
	int mClassNo;
	int mTotalAbnormalCount;
	int mTotalNormalCount;

	std::vector<std::pair<int, int> > mAbnormalIndices, mNormalIndices;
};

struct SplitDataSet {
	BootstrappedDataSet left;
	BootstrappedDataSet right;

	SplitDataSet(BootstrappedDataSet negative, BootstrappedDataSet positive)
		: left(negative), right(positive) {}
};

class AXStageTree : public AXStage
{
public:
	AXStageTree(FuzzyDataSet* fuzzyDataSet,
				ECStage* stage1);

	virtual void initFromEnv(const Environment& env);

	// Функции доступа к набору аксиом
	virtual void setAxioms(const std::vector<std::vector<AxiomExprPlus> > &initial);

	virtual const AxiomExprPlus &getAX(int aType, int n) const;
	virtual bool isAXSelected(int aType, int n) const;
	virtual void setAXSelected(int aType, int n, bool value);
	virtual int getAXSize() const;
	virtual int getAXSize(int aType) const;

	virtual void recalculateMatterAxiomFunc(AxiomExprPlus& ax, int abType);

	virtual void run();

private:

	int numOfAxioms;
	double ecFraction;
	int axiomDepth;
	double threshSplitLevel;
	double maxSplitLevel;

	bool oneVsAll;

	std::vector<std::vector<ElemCondPlus> > elemConds;
	std::vector<std::vector<AxiomExprPlus> > axioms;

	AXTree* constructTree(BootstrappedDataSet dataSet, int depth) const;

	ElemCondPlus chooseEC(const BootstrappedDataSet& dataSet, double &bestIG) const;

	double informationGain(const ElemCondPlus& ec, const BootstrappedDataSet& dataSet, double &splitLevel) const;

	void formAxiomsForClass(int classNo);

};

}

}

#endif // AXSTAGETREE_H
