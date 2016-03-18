#include "ASStageGenetic.h"

#include "AXStage.h"

#include "../ReducedRecognizerFactory.h"
#include "../GoalStrategyFactory.h"

#include "../Logger.h"

#include "../prettyprint.hpp"

#include <shark/Rng/GlobalRng.h>

#include <shark/Algorithms/DirectSearch/Individual.h>
#include <shark/Algorithms/DirectSearch/Operators/Selection/LinearRanking.h>
#include <shark/Algorithms/DirectSearch/FitnessExtractor.h>

#include <string>
#include <iterator>

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

#define QUESTION_MARK_GLOBAL_INDEX -2

AxiomContainer::AxiomContainer(AXStage *stage2, FuzzyDataSet *fuzzyDataSet)
	: mFuzzyDataSet(fuzzyDataSet)
{
	mAxiomInfo.resize(stage2->getAXSize());

	mTotalCount = 0;
	int globalIndex = 0;

	for(int i = 0; i < stage2->getAXSize(); ++i) {
		mAxiomInfo[i].reserve(stage2->getAXSize(i));
		for(int j = 0; j < stage2->getAXSize(i); ++j) {
			if(stage2->isAXSelected(i, j)) {
				AxiomExpr axiomExpr = stage2->getAX(i, j);
				axiomExpr.index = globalIndex++;
				mAxiomInfo[(unsigned)i].push_back(AxiomInfo(axiomExpr, fuzzyDataSet));
			}
		}
		mTotalCount += (int)mAxiomInfo[i].size();
	}
}

int AxiomContainer::totalAxiomCount() const
{
	return mTotalCount;
}

const AxiomExpr &AxiomContainer::axiom(int globalIndex) const
{
	int classNo, i;
	toLocalIndex(globalIndex, classNo, i);
	return axiom(classNo, i);
}

int AxiomContainer::axiomCount(int classNo) const
{
	return (int)mAxiomInfo[classIndex(classNo)].size();
}

const AxiomExpr &AxiomContainer::axiom(int classNo, int i) const
{
	return mAxiomInfo[classIndex(classNo)][i].axiom;
}

int AxiomContainer::classCount() const
{
	return mAxiomInfo.size();
}

bool AxiomContainer::isSat(int axiomClassNo, int axiomNo, int division, int classNumber, int trajectoryNumber, int point) const
{
	return mAxiomInfo[classIndex(axiomClassNo)][axiomNo].satPoints.isSat(division, classNumber, trajectoryNumber, point);
}

bool AxiomContainer::isSat(int globalAxiomIndex, int division, int classNumber, int trajectoryNumber, int point) const
{
	int classNo, i;
	toLocalIndex(globalAxiomIndex, classNo, i);
	return isSat(classNo, i , division, classNumber, trajectoryNumber, point);
}

void AxiomContainer::performMarkUp(const AxiomExprSetPlus &asSet, FuzzyDataSet::DataSetDivisionType division, int classNo, int multiTSNo, std::vector<int> &result) const
{	
	unsigned trajLen = mFuzzyDataSet->getMultiTSLength(division, classNo, multiTSNo);
	result.resize(trajLen);
	std::fill(result.begin(), result.end(), 0);

	for(unsigned t = 0; t < trajLen; ++t) {
		for(unsigned ax = 0; ax < asSet.axioms.size(); ++ax) {
			if(isSat(asSet.axioms[ax]->index, division, classNo, multiTSNo, t)) {
				result[t] = ax + 1;
				break;
			}
		}
	}
}

void AxiomContainer::toLocalIndex(int globalIndex, int &classNo, int &i) const
{
	i = globalIndex;
	for(classNo = 0; classNo < classCount(); ++classNo) {
		if(i < axiomCount(classNo)) {
			return;
		}
		i -= axiomCount(classNo);
	}

	exception() << "AxiomContainer::toLocalIndex() : invalid global axiom index: " << globalIndex
				<< " (total axiom count: " << mTotalCount << ")";
}

int AxiomContainer::classIndex(int classNo)
{
	if(classNo < 0) classNo = -1;
	return classNo;
}

AxiomContainer::ExtendedSatPointSet::ExtendedSatPointSet(const AxiomExpr &axiom, FuzzyDataSet *fuzzyDataSet)
{
	mSatPoints.resize(FuzzyDataSet::Testing - FuzzyDataSet::Reference + 1);
	for(int division = FuzzyDataSet::Reference; division <= FuzzyDataSet::Testing; ++division) {
		mSatPoints[division].reserve(fuzzyDataSet->getClassCount() + 1);
		for(int classNo = -1; classNo < fuzzyDataSet->getClassCount(); ++classNo) {
			mSatPoints[division].push_back(SatPointSet(axiom, *fuzzyDataSet, (FuzzyDataSet::DataSetDivisionType)division, classNo));
		}
	}
}

bool AxiomContainer::ExtendedSatPointSet::isSat(int division, int classNumber, int trajectoryNumber, int point) const
{
	if(classNumber < -1) {
		classNumber = -1;
	}

	return mSatPoints[division][classNumber + 1].isSat(trajectoryNumber, point);
}


std::string ASObjectiveValue::toString() const
{
	return boost::lexical_cast<std::string>(this->goalValue) + " ("
			+ boost::lexical_cast<std::string>(this->errI)
			+ ", "
			+ boost::lexical_cast<std::string>(this->errII)
			+ ")";
}

shark::RealVector ASObjectiveValue::toRealVector() const
{
	shark::RealVector result(2);
	result[0] = errI;
	result[1] = errII;
	return result;
}

ASObjectiveFunction::ASObjectiveFunction(const FuzzyDataSet *fuzzyDataSet)
	:mFuzzyDataSet(fuzzyDataSet), mAxiomContainer(0)
{
}

void ASObjectiveFunction::initFromEnv(const Environment &env)
{
	if (env.getIntParamValue (ccNumPoints, "ccNumPoints") < 0)
		throw AxiomLibException("ASObjectiveFunction::setParamsFromEnv : ccNumPoints is undefined.");

	// создание класса распознавания участков разметки в ряду разметки
	std::string recogClassName;
	ReducedRecognizerFactory rrf;
	if (env.getStringParamValue(recogClassName, "ReducedRecognizer") < 0)
		throw AxiomLibException("FuzzyMultiDataExt_AS::initFromEnv: ReducedRecognizer class is undefined.");
	mReducedRecognizer = std::auto_ptr<ReducedRecognizer>(rrf.create(recogClassName));
	mReducedRecognizer->setParamsFromEnv (env);

	// Инициализируем стратегию вычисления целевой функции
	std::string goalStrategyName;
	GoalStrategyFactory gsf;
	if (env.getStringParamValue(goalStrategyName, "goalClass") < 0)
		throw AxiomLibException("FuzzyMultiDataExt_AS::initFromEnv: goal-class is undefined.");
	mGoalStrategy = std::auto_ptr<GoalStrategy>(gsf.create(goalStrategyName));
	mGoalStrategy->setParamsFromEnv(env);

	env.getParamValue(penaltyObjective, "ASStageGenetic_penaltyObjective", 10000.0);

	env.getParamValue(numAxiomsWeight, "ASStageGenetic_numAxiomsWeight", 0.0);
}

ASObjectiveValue ASObjectiveFunction::eval(const AxiomExprSetPlus &input) const
{
	if(!mAxiomContainer) {
		exception() << "ASObjectiveFunction::eval(): AxiomContainer not set";
	}

	for(unsigned i = 0; i < input.markUps.size(); ++i) {
		if(input.markUps[i].empty()) {
			return ASObjectiveValue();
		}
	}

	AxiomExprSetPlus copy(input);
	matterAxiomSetFunc(copy);
	return ASObjectiveValue(copy.goal + numAxiomsWeight * copy.axioms.size(), copy.errFirst, copy.errSecond);
}

double ASObjectiveFunction::matterAxiomSetFunc(AxiomExprSetPlus &as) const
{
	matterAxiomSetFunc(as, as.markUps);
	return as.goal;
}

double ASObjectiveFunction::matterAxiomSetFunc(AxiomExprSetPlus &as, std::vector<std::vector<int> > &markUps) const
{
	int tmpFirst, tmpSecond;
	double tmpGoal;
	as.errFirst = 0;
	as.errSecond = 0;
	as.errors.resize(markUps.size());
	for (int abType = 0; abType < (int) markUps.size(); abType++) {
		matterAxiomSetFunc (as, abType, markUps[abType], tmpGoal, tmpFirst, tmpSecond);
		as.errors[abType] = std::make_pair(tmpFirst, tmpSecond);
		as.errFirst += tmpFirst;
		as.errSecond += tmpSecond;
	}
	// Вычисление значения целевой функции для полученного числа ошибок I и II рода
	as.goal = mGoalStrategy->compute(as.errFirst, as.errSecond);

	return as.goal;
}

double ASObjectiveFunction::matterAxiomSetFunc (AxiomExprSetPlus &as, int abType, const std::vector <int> &genMarkUp, double &goalVal, int &errFirstVal, int &errSecondVal) const {
	int numOfClasses;
	std::vector <int> numOfMultiTS;
	std::vector < std::vector <int> > numOfTS;
	// получаем информацию о числе траекторий контрольной выборки
	mFuzzyDataSet->getTestSize (numOfClasses, numOfMultiTS, numOfTS);
	if ((abType < 0) || (abType >= numOfClasses))
		throw AxiomLibException("ASObjectiveFunction::matterAxiomSetFunc: incorrect input number of abnormal type.");
	// подготавливаем перменные для хранения числа ошибок
	errFirstVal = 0;
	errSecondVal = 0;
	// Выбираем очередную траекторию для заданного типа нештатного поведения и запускаем распознаватель
	std::vector <int> curMarkUp, curLabeling;
	int num;

//	debug() << "Reference trajectory marking: " << genMarkUp;
//	debug() << "Searching for markings in abnormal trajectories";
	for (int t = 0; t < (int) numOfTS[abType].size(); t++) {
		int currentFirstKindErrors = 0;
		int currentSecondKindErrors = 0;

//		debug() << "Abnormal trajectory marking: " << curMarkUp;

		// разметка траектории контрольной выборки системой аксиом as
		performMarkUp(as, FuzzyDataSet::Testing, abType, t, curMarkUp);
		// Распознавание нештатного поведения в разметке ряда
		mReducedRecognizer->run (curMarkUp, genMarkUp, curLabeling);

//		debug() << "Resulting labeling: " << curLabeling;

		// Вычисление числа ошибок первого и второго рода
		num = getStatistic (curLabeling);

		// Суммирование числа ошибок
		if (num == 0) {
			currentSecondKindErrors = 1;
		}
		else {
			currentFirstKindErrors = num - 1;
		}

		as.setErrorsForTraj(abType, t, currentFirstKindErrors, currentSecondKindErrors);

		errSecondVal+= currentSecondKindErrors;
		errFirstVal += currentFirstKindErrors;
	}
	// Пробегаем по всем рядам нормального поведения
	int numNormalMultiTS;
	std::vector <int> numNormalTS;
	mFuzzyDataSet->getNormalTestSize (numNormalMultiTS, numNormalTS);

//	debug() << "Searching for markings in normal trajectory";

	for (int t = 0; t < (int) numNormalTS.size(); t++) {
		// размечаем траекторию нормального поведения
		performMarkUp(as, FuzzyDataSet::Testing, -1, t, curMarkUp);

//		debug() << "Normal trajectory marking: " << curMarkUp;
		// Распознавание нештатного поведения в разметке ряда
		mReducedRecognizer->run (curMarkUp, genMarkUp, curLabeling);

//		debug() << "Resulting labeling: " << curLabeling;

		// Вычисление числа ошибок первого и второго рода
		num = getStatistic (curLabeling);

		// Суммирование числа ошибок
		errFirstVal += num;

		int oldTypeIErrors = as.getErrorsForTraj(-1, t).first;
		if(oldTypeIErrors < 0) oldTypeIErrors = 0;

		as.setErrorsForTraj(-1, t, num + oldTypeIErrors, 0);
	}

	// Вычисление значения целевой функции для полученного числа ошибок I и II рода
	goalVal = mGoalStrategy->compute(errFirstVal, errSecondVal);

	return goalVal;
}

void ASObjectiveFunction::performMarkUp(const AxiomExprSetPlus &as, FuzzyDataSet::DataSetDivisionType division, int classNo, int multiTSNo, std::vector<int> &result) const
{
	mAxiomContainer->performMarkUp(as, division, classNo,  multiTSNo, result);
}

int ASObjectiveFunction::getStatistic(std::vector<int> &row) const
{
	int num = 0;
	int i = 0;
	int j;
	int len = (int) row.size();
	while (i < len) {
		if (row[i] > 0) {
			num++;
			j = i+1;
			while (j < len) {
				if (row[j]!=row[j-1])
					break;
				j++;
			}
			if ((j - i) > ccNumPoints)
				num += j - i - ccNumPoints;
			i = j;
		} else {
			i++;
		}
	}
	return num;
}


void ASMutation::operator ()(ASIndividual &i) const
{
	AxiomExprSetPlus& as = *i;
	int classToMutate = 0;
	if(as.markUps.size() > 1)
	{
		classToMutate = shark::Rng::discrete(0, as.markUps.size() - 1);
	}

	int action;
	if((*i).axioms.empty() || as.markUps[classToMutate].empty()) {
		action = AddForeign;
	} else {
		action = shark::Rng::discrete(FirstAction, LastAction);
	}

	switch(action) {
		case AddOwn: {
			int newAxiomNo = shark::Rng::discrete(1, as.axioms.size());
			int newPosition = shark::Rng::discrete(0, as.markUps[classToMutate].size());
			addAxiom(as, classToMutate, as.axioms[newAxiomNo - 1]->index, newPosition);
		} break;
		case AddForeign: {
			int newAxiomGlobalIndex = shark::Rng::discrete(0, mAxiomContainer->totalAxiomCount() - 1);
			int newPosition = shark::Rng::discrete(0, as.markUps[classToMutate].size());
			addAxiom(as, classToMutate, newAxiomGlobalIndex, newPosition);
		} break;
		case ReplaceWithOwn: {
			int newAxiomNo = shark::Rng::discrete(1, as.axioms.size());
			int newGlobalAxiomIndex = as.axioms[newAxiomNo - 1]->index;
			int newPosition = shark::Rng::discrete(0, as.markUps[classToMutate].size() - 1);
			removeAxiom(as, classToMutate, newPosition);
			addAxiom(as, classToMutate, newGlobalAxiomIndex, newPosition);
		} break;
        case AddQuestionMarkSymbol: {
            int newPosition = shark::Rng::discrete(0, as.markUps[classToMutate].size() - 1);
            removeAxiom(as, classToMutate, newPosition);
            addAxiom(as, classToMutate, QUESTION_MARK_GLOBAL_INDEX, newPosition);
        } break;
		case ReplaceWithForeign: {
			int newAxiomGlobalIndex = shark::Rng::discrete(0, mAxiomContainer->totalAxiomCount() - 1);
			int newPosition = shark::Rng::discrete(0, as.markUps[classToMutate].size() - 1);
			removeAxiom(as, classToMutate, newPosition);
			addAxiom(as, classToMutate, newAxiomGlobalIndex, newPosition);
		}
		case Remove: {
			if(as.markUps[classToMutate].size() > 1) {
				removeAxiom(as, classToMutate);
			}
		} break;
	}
}

void ASMutation::addAxiom(AxiomExprSetPlus &as, int classNo, int newAxiomGlobalIndex, int position) const
{
	std::vector<int>& marking = as.markUps[classNo];

	int newAxiomLocalIndex = 0;
	for(unsigned i = 0; i < as.axioms.size(); ++i) {
		if(as.axioms[i]->index == newAxiomGlobalIndex) {
            newAxiomLocalIndex = i + 1;
		}
	}
	if(!newAxiomLocalIndex) {
		as.addAxiom(mAxiomContainer->axiom(newAxiomGlobalIndex));
		newAxiomLocalIndex = as.size() - 1;
	}
	marking.insert(marking.begin() + position, newAxiomLocalIndex);
}

void ASMutation::removeAxiom(AxiomExprSetPlus &as, int classNo, int position) const
{
	std::vector<int>& marking = as.markUps[classNo];

	if(position < 0) {
		position = shark::Rng::discrete(0, marking.size() - 1);
	}

	int axiomToRemove = marking[position];
	marking.erase(marking.begin() + position);

	if(axiomToRemove != 0 && std::find(marking.begin(), marking.end(), axiomToRemove) == marking.end()) {
		// Если такой аксиомы в разметке больше нет, удаляем ее из системы аксиом
		as.removeAxiom(axiomToRemove);
	}
}

void ASOnePointCrossover::operator ()(const ASIndividual &ind1, const ASIndividual &ind2, ASIndividual &off1, ASIndividual &off2) const
{
	int classToCrossover = 0;
	if(mAxiomContainer->classCount() > 1) {
		//TODO multiple classes
	}

	off1 = ind1;
	off2 = ind2;

	std::vector<int> ind1Marking = makeGlobalMarkings((*ind1), classToCrossover);
	std::vector<int> ind2Marking = makeGlobalMarkings((*ind2), classToCrossover);

	std::vector<int> off1Marking, off2Marking;

	onePointCrossover(ind1Marking, ind2Marking, off1Marking, off2Marking);

	addLocalMarking((*off1), classToCrossover, off1Marking);
	addLocalMarking((*off2), classToCrossover, off2Marking);
}

std::vector<int> ASOnePointCrossover::makeGlobalMarkings(const AxiomExprSetPlus &as, int classNo) const
{
	const std::vector<int>& marking = as.markUps[classNo];

	std::vector<int> result(marking.size());

	for(unsigned i = 0; i < result.size(); ++i) {
        result[i] = marking[i] > 0 ? as.axioms[(unsigned)marking[i] - 1]->index : -1;
	}

    return result;
}

void ASOnePointCrossover::addLocalMarking(AxiomExprSetPlus &as, int classNo, const std::vector<int> &globalMarking) const
{
	std::map<int, unsigned> existingAxioms; // global -> local
	existingAxioms.insert(std::make_pair(-1, 0u));

	for(unsigned i = 0; i < as.axioms.size(); ++i) {
        existingAxioms.insert(std::make_pair(as.axioms[i]->index, i + 1));
	}

	for(unsigned p = 0; p < globalMarking.size(); ++p) {
		if(!existingAxioms.count(globalMarking[p])) {
			as.addAxiom(mAxiomContainer->axiom(globalMarking[p]));
		}
	}

	existingAxioms.clear();
	existingAxioms.insert(std::make_pair(-1, 0u));
	for(unsigned i = 0; i < as.axioms.size(); ++i) {
		existingAxioms.insert(std::make_pair(as.axioms[i]->index, i + 1));
	}

	as.markUps[classNo].resize(globalMarking.size());
	for(unsigned p = 0; p < globalMarking.size(); ++p) {
		as.markUps[classNo][p] = existingAxioms[globalMarking[p]];
	}
}

void ASOnePointCrossover::onePointCrossover(const std::vector<int> &dad, const std::vector<int> &mom, std::vector<int> &off1, std::vector<int> &off2) const
{
	unsigned dadCrossoverPoint = (unsigned)shark::Rng::discrete(0, dad.size());
	unsigned momCrossoverPoint = (unsigned)shark::Rng::discrete(0, mom.size());

	off1.clear();
	off1.reserve(dad.size() + mom.size());

	off2.clear();
	off2.reserve(dad.size() + mom.size());

	for(unsigned i = 0; i < dadCrossoverPoint; ++i) {
		off1.push_back(dad[i]);
	}

	for(unsigned i = 0; i < momCrossoverPoint; ++i) {
		off2.push_back(mom[i]);
	}

	for(unsigned i = momCrossoverPoint; i < mom.size(); ++i) {
		off1.push_back(mom[i]);
	}

	for(unsigned i = dadCrossoverPoint; i < dad.size(); ++i) {
		off2.push_back(dad[i]);
	}
}

ASStageGenetic::ASStageGenetic(FuzzyDataSet* fuzzyDataSet,
							   AXStage* stage2)
	: stage2(stage2), fuzzyDataSet(fuzzyDataSet), mObjective(fuzzyDataSet)

{
}

void ASStageGenetic::initFromEnv(const Environment &env)
{
	env.getMandatoryParamValue(mPopulationSize, "ASStageGenetic_populationSize");
	env.getParamValue(mInitialMarkingLength, "ASStageGenetic_initialMarkingLength", 10);
	env.getParamValue(mInitialASSize, "ASStageGenetic_initialASSize", 5);
	env.getMandatoryParamValue(mMaxIterations, "ASStageGenetic_maxIterations");
	env.getMandatoryParamValue(mElitism, "ASStageGenetic_elitism");
	env.getMandatoryParamValue(mSelectivePressure, "ASStageGenetic_selectivePressure");
	if(mSelectivePressure < 1.0 || mSelectivePressure > 2.0) {
		exception() << "ASStageGenetic::initFromEnv() : invalid selectivePressure value: " << mSelectivePressure
					<< ", selective pressure must be from [1, 2]";
	}

	env.getParamValue(mFinalAxiomSetCount, "ASStageGenetic_finalAxiomSetCount", 40);

	mObjective.initFromEnv(env);
}

template<class Extractor>
struct FitnessComparator {
	explicit FitnessComparator(const Extractor& extractor = Extractor()): mExtractor(extractor) {}

	template<class Individual>
	bool operator()(const Individual& first, const Individual& second) const {
		return mExtractor(first) < mExtractor(second);
	}

private:
	Extractor mExtractor;
};

struct FitnessExtractor {
	template<class Individual>
	double operator()(const Individual& ind) const {
		return ind.penalizedFitness()[0];
	}
};

void printNormalTrajectoryMarking(FuzzyDataSet* fuzzyDataSet, const AxiomExprSet& as) {
	std::vector<std::vector<double> > traj;
	std::vector<int> marking;
	fuzzyDataSet->getMultiTSByIndex(FuzzyDataSet::Testing, traj, -1, 0);
	as.enter(marking, traj, 0, fuzzyDataSet->getMultiTSLength(FuzzyDataSet::Testing, -1, 0));
//	debug() << "Normal trajectory: " << traj;
//	debug() << "Normal trajectory marking: " << marking;
}

void ASStageGenetic::run()
{
	AxiomContainer* axiomContainer = new AxiomContainer(stage2, fuzzyDataSet);

	mObjective.setAxiomContainer(axiomContainer);

	std::vector<ASIndividual> population;

	int elitismNumber = (int) (mElitism * mPopulationSize);

	ASMutation mutation(axiomContainer);
	ASOnePointCrossover crossover(axiomContainer);

	shark::LinearRankingSelection<FitnessExtractor> selection;
	selection.etaMax = mSelectivePressure;

	comment() << "Starting ASStageGenetic algorithm";

	population.resize(mPopulationSize);
	for(unsigned i = 0; i < population.size(); ++i) {
		*population[i] = generateInitialAS(axiomContainer);
		ASObjectiveValue objectiveValue = mObjective.eval(*(population[i]));
		population[i].penalizedFitness() = shark::RealVector(1, objectiveValue.goalValue);
	}

	std::sort(population.begin(), population.end(), FitnessComparator<FitnessExtractor>());

	comment() << "Initial population best goal value: " << mObjective.eval(*population.front()).toString();

	population.reserve(4*mPopulationSize);

	for(int i = 0; i < mMaxIterations; ++i) {

		std::sort(population.begin(), population.end(), FitnessComparator<FitnessExtractor>());

		comment() << "Iteration number: " << (i + 1);

		comment() << "Best goal value: " << mObjective.eval(*population.front()).toString();
		debug() << "Performing mutation...";
		std::copy(population.begin(), population.end(), std::back_inserter(population));
		// Mutation
		for(unsigned p = mPopulationSize; p < 2 * mPopulationSize; ++p) {
			//TODO mutation strength, mutation probability
			mutation(population[p]);
		}

		debug() << "Performing crossover...";
		// Crossover
		for(unsigned p = 0; p < mPopulationSize; ++p) {
			ASIndividual off1, off2;
			unsigned dadIndex = (unsigned)shark::Rng::discrete(0, 2*(int)mPopulationSize - 1);
			unsigned momIndex = (unsigned)shark::Rng::discrete(0, 2*(int)mPopulationSize - 1);

			const ASIndividual& dad = population[dadIndex];

			const ASIndividual& mom = population[momIndex];

			crossover(dad, mom, off1, off2);

			population.push_back(off1);
			population.push_back(off2);
		}

		debug() << "Re-evaluating objective function for extended population...";
		// Evaluate objective function
		for(unsigned i = mPopulationSize; i < population.size(); ++i) {
			population[i].penalizedFitness() =
					shark::RealVector(1, mObjective.eval(*(population[i])).goalValue);
		}

		// Selection
		std::sort(population.begin(), population.end(), FitnessComparator<FitnessExtractor>());

		debug() << "Performing selection...";

		selection(population.begin() + elitismNumber, population.end(),
				  population.begin() + elitismNumber, population.begin() + mPopulationSize);

		population.resize(mPopulationSize);

	}

	std::sort(population.begin(), population.end(), FitnessComparator<FitnessExtractor>());

	comment() << "Final best goal value: " << mObjective.eval(*population.front()).toString();

	printNormalTrajectoryMarking(fuzzyDataSet, *population.front());

	mFinalAxiomSets.clear();
	for(int i = 0; i < std::min(mFinalAxiomSetCount, (int)population.size()); ++i) {
		AxiomExprSetPlus as = *population[i];
		mObjective.matterAxiomSetFunc(as);
		mFinalAxiomSets.push_back(as);
	}

	mObjective.setAxiomContainer(0);

	delete axiomContainer;
}

AxiomExprSetPlus &ASStageGenetic::getAS(int n)
{
	return mFinalAxiomSets[n];
}

const AxiomExprSetPlus &ASStageGenetic::getAS(int n) const
{
	return mFinalAxiomSets[n];
}

int ASStageGenetic::getASSize() const
{
	return mFinalAxiomSets.size();
}

void ASStageGenetic::recalculateMatterASFunc(AxiomExprSetPlus &as)
{
	mObjective.matterAxiomSetFunc(as);
}

void ASStageGenetic::setAxiomSets(const std::vector<AxiomExprSetPlus> &initialAS)
{
	mFinalAxiomSets = initialAS;
}

void trimZeros(std::vector<int> & v) {
	// Убираем все 0 в начале вектора - их не учитываем
	int k, l;
	k = 0;
	while (k < (int) v.size()) {
		if (v[k] > 0)
			break;
		k++;
	}
	// не учитываем все 0 в конце вектора разметки
	l = (int) v.size() - 1;
	while (l >= k) {
		if (v[l] > 0)
			break;
		l--;
	}
	if (l < k) {
		// Если были только 0 - делаем разметку из одного 0
		v.resize(1, 0);
	} else {
		// Удаляем 0 в начале и в конце вектора разметки
		v.erase(v.begin() + l + 1, v.end());
		v.erase(v.begin(), v.begin() + k);
	}
}

AxiomExprSetPlus ASStageGenetic::generateInitialAS(const AxiomContainer *axiomContainer) const
{
	AxiomExprSetPlus result("");

	int classCount = axiomContainer->classCount();

	result.markUps.resize(classCount);
	for(int i = 0; i < axiomContainer->classCount(); ++i) {
		int firstAxiomNumber = result.axioms.size() + 1;
		for(int j = 0; j < mInitialASSize; ++j) {
			int axNo = shark::Rng::discrete(0, axiomContainer->axiomCount(i) - 1);
			AxiomExpr newAxiom = axiomContainer->axiom(i, axNo);
			result.addAxiom(newAxiom);
		}

		int lastAxiomNumber = result.axioms.size();

		int coin = shark::Rng::discrete(0, 1);
		if(coin) {
//			debug() << "Using emergency trajectory marking as reference marking";
			std::vector<int> marking;
			int trajectoryNo = shark::Rng::discrete(0, fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Reference, i) - 1);
			axiomContainer->performMarkUp(result, FuzzyDataSet::Reference, i, trajectoryNo, marking);
			trimZeros(marking);
			result.markUps[i] = marking;
		} else {
//			debug() << "Using single-axiom marking as initial reference marking";
			int axiomNum = shark::Rng::discrete(firstAxiomNumber - 1, lastAxiomNumber);
			if(axiomNum < firstAxiomNumber) axiomNum = 0;
			result.markUps[i].push_back(axiomNum);
		}
	}

	return result;
}
