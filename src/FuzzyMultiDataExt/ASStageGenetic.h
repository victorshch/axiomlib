#ifndef ASSTAGEGENETIC_H
#define ASSTAGEGENETIC_H

#include "../AxiomExprSetPlus.h"
#include "../SatPointSet.h"

#include "ASStage.h"
#include "shark/ObjectiveFunctions/AbstractObjectiveFunction.h"
#include "shark/Algorithms/DirectSearch/TypedIndividual.h"
#include "../GoalStrategy.h"
#include "../ReducedRecognizer.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class AxiomContainer {
public:
	AxiomContainer(AXStage* stage2, FuzzyDataSet* fuzzyDataSet);

	int totalAxiomCount() const;
	const AxiomExpr& axiom(int globalIndex) const;

	int axiomCount(int classNo) const;
	const AxiomExpr& axiom(int classNo, int i) const;

	int classCount() const;

	bool isSat(int axiomClassNo, int axiomNo, int division, int classNumber, int trajectoryNumber, int point) const;
	bool isSat(int globalAxiomIndex, int division, int classNumber, int trajectoryNumber, int point) const;

	int toGlobalIndex(int classNo, int i) const { return axiom(classNo, i).index; }
private:
	void toLocalIndex(int globalIndex, int& classNo, int& i) const;

	static int classIndex(int classNo);

	class ExtendedSatPointSet {
	public:
		ExtendedSatPointSet(const AxiomExpr& axiom, FuzzyDataSet* fuzzyDataSet);

		bool isSat(int division, int classNumber,  int trajectoryNumber, int point) const;

	private:
		std::vector<std::vector<SatPointSet> > mSatPoints;
	};

	struct AxiomInfo {
		AxiomExpr axiom;

		ExtendedSatPointSet satPoints;

		AxiomInfo(const AxiomExpr& axiom, FuzzyDataSet* fuzzyDataSet)
			: axiom(axiom), satPoints(axiom, fuzzyDataSet) {}
	};

	std::vector<std::vector<AxiomInfo> > mAxiomInfo;

	int mTotalCount;
};

struct ASSpace {
	BOOST_STATIC_CONSTANT( bool, IS_VECTOR_SPACE = false );

	typedef AxiomExprSetPlus PointType;
};

class ASObjectiveFunction: public shark::AbstractObjectiveFunction<ASSpace, double> {
public:
	ASObjectiveFunction(const FuzzyDataSet* fuzzyDataSet, const AxiomContainer* axiomContainer);

	void initFromEnv(const Environment& env);

	double eval(const AxiomExprSetPlus &input) const;

	double matterAxiomSetFunc(AxiomExprSetPlus& as) const;

private:
	double matterAxiomSetFunc (AxiomExprSetPlus &as, std::vector <std::vector <int> > &markUps) const;

	double matterAxiomSetFunc (AxiomExprSetPlus &as, int abType, const std::vector <int> &genMarkUp, double &goalVal, int &errFirstVal, int &errSecondVal) const;

	void performMarkUp(const AxiomExprSetPlus &as, FuzzyDataSet::DataSetDivisionType, int classNo, int multiTSNo, std::vector<int>& result) const;

	int getStatistic (std::vector <int> &row) const;

	std::auto_ptr<GoalStrategy> mGoalStrategy;
	std::auto_ptr<ReducedRecognizer> mReducedRecognizer;
	const FuzzyDataSet* mFuzzyDataSet;
	const AxiomContainer *mAxiomContainer;

	int ccNumPoints;
};

/// Returns indexCount random numbers from 0 to size-1 without repetitions
std::vector<int> randomIndices(int size, int indexCount);

typedef shark::TypedIndividual< AxiomExprSetPlus > ASIndividual;

struct ASMutation {
	ASMutation(const AxiomContainer* axiomContainer): mAxiomContainer(axiomContainer) {}

	void operator()(ASIndividual& i) const;
	//TODO

private:
	enum MutationAction {
		ReplaceWithOwn,
		FirstAction = ReplaceWithOwn,
		ReplaceWithForeign,
		AddOwn,
		AddForeign,
		Remove,
		LastAction = Remove
	};

	void addAxiom(AxiomExprSetPlus& as, int classNo, int newAxiomGlobalIndex, int position) const;

	// position = -1 means random
	void removeAxiom(AxiomExprSetPlus& as, int classNo, int position = -1) const;

	const AxiomContainer* mAxiomContainer;
};

struct ASOnePointCrossover {
	ASOnePointCrossover(const AxiomContainer* axiomContainer): mAxiomContainer(axiomContainer) {}

	void operator()(const ASIndividual& ind1, const ASIndividual& ind2, ASIndividual& off1, ASIndividual& off2) const;

private:
	// Переводит разметки, в которых указан номер аксиомы в системе аксиом,
	// в разметки, в которых указан глобальный индекс аксиомы или -1 для тождественной аксиомы
	std::vector<int> makeGlobalMarkings(const AxiomExprSetPlus& as, int classNo) const;

	void addLocalMarking(AxiomExprSetPlus& as, int classNo, const std::vector<int>& globalMarking) const;

	void onePointCrossover(const std::vector<int>& dad, const std::vector<int>& mom,
						   std::vector<int>& off1, std::vector<int>& off2) const;

	const AxiomContainer* mAxiomContainer;
};

/// Стадия построения системы аксиом и разметок эталонных траекторий по готовым аксиомам,
/// использующая генетический алгоритм, в котором особью является (система аксиом + набор разметок)
class ASStageGenetic : public ASStage
{
public:
	ASStageGenetic(FuzzyDataSet* fuzzyDataSet,
				   AXStage* stage2);

	void initFromEnv(const Environment &env);

	void run();

	virtual AxiomExprSetPlus &getAS(int n);
	virtual const AxiomExprSetPlus &getAS(int n) const;
	virtual int getASSize() const;
	virtual void recalculateMatterASFunc(AxiomExprSetPlus& as);
	virtual void setAxiomSets(const std::vector<AxiomExprSetPlus>& initialAS);
private:
	/// Генерация случайной системы аксиом для начальной популяции
	AxiomExprSetPlus generateInitialAS() const;


	int mPopulationSize;
	int mInitialMarkingLength;
	int mMaxIterations;

	double mElitism;

	double mSelectivePressure;

	double mMinMutationRate, mMaxMutationRate;
	double mMinCrossoverRate, mMaxCrossoverRate;

	int mMaxAxiomCount;

	int mFinalAxiomSetCount;

	AxiomContainer mAxiomContainer;
	ASObjectiveFunction mObjective;

	std::vector<AxiomExprSetPlus> mFinalAxiomSets;

};


}

}

#endif // ASSTAGEGENETIC_H
