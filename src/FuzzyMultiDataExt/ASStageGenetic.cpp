#include "ASStageGenetic.h"

#include "AXStage.h"

#include "../ReducedRecognizerFactory.h"
#include "../GoalStrategyFactory.h"

#include "../Logger.h"

#include <shark/Algorithms/DirectSearch/TypedIndividual.h>
#include <shark/Algorithms/DirectSearch/FitnessComparator.h>
#include <shark/Rng/GlobalRng.h>
#include <shark/Algorithms/DirectSearch/Operators/Selection/LinearRanking.h>

#include <string>

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

AxiomContainer::AxiomContainer(AXStage *stage2, FuzzyDataSet *fuzzyDataSet)
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
	return mAxiomInfo.size() - 1;
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

void AxiomContainer::toLocalIndex(int globalIndex, int &classNo, int &i) const
{
	i = globalIndex;
	for(classNo = -1; classNo < classCount(); ++classNo) {
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
	return classNo + 1;
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

ASObjectiveFunction::ASObjectiveFunction(const FuzzyDataSet *fuzzyDataSet, const AxiomContainer *axiomContainer)
	:mFuzzyDataSet(fuzzyDataSet), mAxiomContainer(axiomContainer)
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
}

std::string goalStr(const AxiomExprSetPlus& as) {
	return boost::lexical_cast<std::string>(as.goal) + " ("
			+ boost::lexical_cast<std::string>(as.errFirst)
			+ ", "
			+ boost::lexical_cast<std::string>(as.errSecond)
			+ ")";
}

double ASObjectiveFunction::eval(const AxiomExprSetPlus &input) const
{
	AxiomExprSetPlus copy(input);
	matterAxiomSetFunc(copy);
	return copy.goal;
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
	for (int t = 0; t < (int) numOfTS[abType].size(); t++) {
		int currentFirstKindErrors = 0;
		int currentSecondKindErrors = 0;
		// разметка траектории контрольной выборки системой аксиом as
		performMarkUp(as, FuzzyDataSet::Testing, abType, t, curMarkUp);

		// Распознавание нештатного поведения в разметке ряда
		mReducedRecognizer->run (curMarkUp, genMarkUp, curLabeling);

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
	for (int t = 0; t < (int) numNormalTS.size(); t++) {
		// размечаем траекторию нормального поведения
		performMarkUp(as, FuzzyDataSet::Testing, -1, t, curMarkUp);

		// Распознавание нештатного поведения в разметке ряда
		mReducedRecognizer->run (curMarkUp, genMarkUp, curLabeling);

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
	int multiTSLen = mFuzzyDataSet->getMultiTSLength(division, classNo, multiTSNo);

	result.resize(multiTSLen);
	std::fill(result.begin(), result.end(), 0);

	for(unsigned int i = 0; i < as.axioms.size(); i++) {
		for(int j = 0; j < multiTSLen; j++) {
			if(mAxiomContainer->isSat(as.axioms[i]->index, division, classNo, multiTSNo, j) && result[j] == 0) {
				result[j] = i + 1;
			}
		}
	}
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
	int action = shark::Rng::discrete(FirstAction, LastAction);
	int classToMutate = 0;
	if(as.markUps.size() > 1) {
		classToMutate = shark::Rng::discrete(0, as.markUps.size() - 1);
	}
	switch(action) {
		case AddOwn: {
			int newAxiomNo = shark::Rng::discrete(1, as.axioms.size());
			int newPosition = shark::Rng::discrete(0, as.markUps[classToMutate].size());
			addAxiom(as, classToMutate, as.axioms[newAxiomNo]->index, newPosition);
		} break;
		case AddForeign: {
			int newAxiomGlobalIndex = shark::Rng::discrete(0, mAxiomContainer->totalAxiomCount() - 1);
			int newPosition = shark::Rng::discrete(0, as.markUps[classToMutate].size());
			addAxiom(as, classToMutate, newAxiomGlobalIndex, newPosition);
		} break;
		case ReplaceWithOwn: {
			int newAxiomNo = shark::Rng::discrete(1, as.axioms.size());
			int newPosition = shark::Rng::discrete(0, as.markUps[classToMutate].size() - 1);
			removeAxiom(as, classToMutate, newPosition);
			addAxiom(as, classToMutate, as.axioms[newAxiomNo]->index, newPosition);
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
	for(int i = 0; i < as.axioms.size(); ++i) {
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

	if(std::find(marking.begin(), marking.end(), axiomToRemove) == marking.end()) {
		// Если такой аксиомы в разметке больше нет, удаляем ее из системы аксиом
		as.removeAxiom(axiomToRemove);
	}
}

ASStageGenetic::ASStageGenetic(FuzzyDataSet* fuzzyDataSet,
							   AXStage* stage2)
	:  mAxiomContainer(stage2, fuzzyDataSet), mObjective(fuzzyDataSet, &mAxiomContainer)
{
}

void ASStageGenetic::initFromEnv(const Environment &env)
{
	env.getMandatoryParamValue(mPopulationSize, "ASStageGenetic_populationSize");
	env.getParamValue(mInitialMarkingLength, "ASStageGenetic_initialMarkingLength", 10);
	env.getMandatoryParamValue(mMaxIterations, "ASStageGenetic_maxIterations");
	env.getMandatoryParamValue(mElitism, "ASStageGenetic_elitism");
	env.getMandatoryParamValue(mSelectivePressure, "ASStageGenetic_selectivePressure");
	if(mSelectivePressure < 1.0 || mSelectivePressure > 2.0) {
		exception() << "ASStageGenetic::initFromEnv() : invalid selectivePressure value: " << mSelectivePressure
					<< ", selective pressure must be from [1, 2]";
	}

	env.getParamValue(mFinalAxiomSetCount, "ASStageGenetic_finalAxiomSetCount", 40);
}

void ASStageGenetic::run()
{
	std::vector<ASIndividual> population, offspring;

	int elitismNumber = (int) mElitism * mPopulationSize;

	ASMutation mutation(&mAxiomContainer);

	shark::LinearRankingSelection<shark::tag::UnpenalizedFitness> selection;

	comment() << "Starting ASStageGenetic algorithm";

	population.resize(mPopulationSize);
	for(unsigned i = 0; i < population.size(); ++i) {
		*population[i] = generateInitialAS();
		population[i].fitness(shark::tag::UnpenalizedFitness()) = shark::RealVector(1, mObjective.eval(*(population[i])));
	}

	std::sort(population.begin(), population.end(), shark::FitnessComparator<shark::tag::UnpenalizedFitness>());

	AxiomExprSetPlus bestAS = *population.front();

	mObjective.matterAxiomSetFunc(bestAS);

	comment() << "Initial population best goal value: " << goalStr(bestAS);

	for(int i = 0; i < mMaxIterations; ++i) {

		std::sort(population.begin(), population.end(), shark::FitnessComparator<shark::tag::UnpenalizedFitness>());

		comment() << "Iteration number: " << i;

		bestAS = *population.front();
		mObjective.matterAxiomSetFunc(bestAS);

		comment() << "Best goal value: " << goalStr(bestAS);

		offspring = population;
		// Mutation
		for(unsigned p = 0; p < offspring.size(); ++p) {
			//TODO mutation strength, mutation probability
			mutation(offspring[p]);
		}

		// Crossover
		// TODO

		// Selection
		std::sort(offspring.begin(), offspring.end(), shark::FitnessComparator<shark::tag::UnpenalizedFitness>());

		// elitism
		std::copy(offspring.begin(), offspring.begin() + elitismNumber, population.begin());

		selection(population.begin() + elitismNumber, population.end(), offspring.begin() + elitismNumber, offspring.end(),
				  population.begin() + elitismNumber, population.end(), mSelectivePressure);

	}

	std::sort(population.begin(), population.end(), shark::FitnessComparator<shark::tag::UnpenalizedFitness>());

	bestAS = *population.front();
	mObjective.matterAxiomSetFunc(bestAS);
	comment() << "Final best goal value: " << goalStr(bestAS);

	mFinalAxiomSets.clear();
	for(int i = 0; i < std::min(mFinalAxiomSetCount, (int)population.size()); ++i) {
		mFinalAxiomSets.push_back(*population[i]);
	}
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

AxiomExprSetPlus ASStageGenetic::generateInitialAS() const
{
	AxiomExprSetPlus result("");

	// Generate AS with single random axiom for each class
	// Maybe it's not a very good idea
	result.markUps.resize(mAxiomContainer.classCount());
	for(int i = 0; i < mAxiomContainer.classCount(); ++i) {
		AxiomExpr newAxiom = mAxiomContainer.axiom(i, shark::Rng::discrete(0, mAxiomContainer.axiomCount(i) - 1));
		result.addAxiom(newAxiom);
		result.markUps[i] = std::vector<int>(1, i + 1);
	}

	return result;
}

void ASOnePointCrossover::operator ()(const ASIndividual &ind1, const ASIndividual &ind2, ASIndividual &off1, ASIndividual &off2) const
{
	int classToCrossover = 0;
	if(mAxiomContainer->classCount() > 1) {
		//TODO multiple classes
	}

	off1 = ind1;
	off2 = ind2;

	std::vector<int> ind1Marking = makeGlobalMarkings((*off1), classToCrossover);
	std::vector<int> ind2Marking = makeGlobalMarkings((*off2), classToCrossover);

	std::vector<int> off1Marking, off2Marking;

	onePointCrossover(ind1Marking, ind2Marking, off1Marking, off2Marking);

	addLocalMarking((*off1), classToCrossover, off1Marking);
	addLocalMarking((*off2), classToCrossover, off2Marking);
}

std::vector<int> ASOnePointCrossover::makeGlobalMarkings(const AxiomExprSetPlus &as, int classNo) const
{
	const std::vector<int>& marking = as.markUps[classNo];

	std::vector<int> result(marking.size());

	for(int i = 0; i < result.size(); ++i) {
		result[i] = marking[i] > 0 ? as.axioms[marking[i] - 1].index : -1;
	}

	return result;
}

void ASOnePointCrossover::addLocalMarking(AxiomExprSetPlus &as, int classNo, const std::vector<int> &globalMarking) const
{
	std::map<int, int> existingAxioms; // global -> local
	for(int i = 0; i < as.axioms.size(); ++i) {
		existingAxioms.insert(std::make_pair(as.axioms[i].index, i + 1));
	}

	for(int p = 0; p < globalMarking.size(); ++p) {
		if(!existingAxioms.count(globalMarking[p])) {
			as.addAxiom(mAxiomContainer->axiom(globalMarking[p]));
		}
	}

	existingAxioms.clear();
	for(int i = 0; i < as.axioms.size(); ++i) {
		existingAxioms.insert(std::make_pair(as.axioms[i].index, i + 1));
	}

	as.markUps[classNo].resize(globalMarking.size());
	for(int p = 0; p < globalMarking.size(); ++p) {
		as.markUps[classNo][p] = existingAxioms[globalMarking[p]];
	}
}

void ASOnePointCrossover::onePointCrossover(const std::vector<int> &dad, const std::vector<int> &mom, std::vector<int> &off1, std::vector<int> &off2) const
{
}
