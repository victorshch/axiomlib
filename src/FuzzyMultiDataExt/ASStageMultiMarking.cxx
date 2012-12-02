#include "ASStageMultiMarking.h"
#include "../ReducedRecognizerFactory.h"
#include "../GoalStrategyFactory.h"
#include "../Logger.h"

#include "AXStage.h"

#include "ASStage.h"

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

ASStageMultiMarking::ASStageMultiMarking(){}

void ASStageMultiMarking::initFromEnv(const Environment& env){}

void ASStageMultiMarking::run(){}

AxiomExprSetPlus &ASStageMultiMarking::getAS(int n){}

const AxiomExprSetPlus &ASStageMultiMarking::getAS(int n) const{}

int ASStageMultiMarking::getASSize() const{}

void ASStageMultiMarking::recalculateMatterASFunc(AxiomExprSetPlus& as){}

void ASStageMultiMarking::setAxiomSets(const std::vector<AxiomExprSetPlus>& initialAS){}
