#include "AXStageCombined.h"

#include "../Logger.h"

using namespace AxiomLib;
using namespace FuzzyMultiDataExt;

AXStageCombined::~AXStageCombined()
{
	for(unsigned i = 0; i < mComponents.size(); ++i) {
		delete mComponents[i];
	}
}

void AXStageCombined::initFromEnv(const Environment &env)
{
	std::set<std::string> names;
	env.getStringSetParamValue(names, "AXStageComponent");

	mComponents.clear();

	for(std::set<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
		AXStage* stage = AXStage::create(*it, fuzzyDataSet, stage1);
		if(!stage) {
			throw AxiomLibException(" AXStageCombined::initFromEnv(): Invalid stage name: " + *it);
		}
		stage->initFromEnv(env);
		mComponents.push_back(stage);
	}
}

void AXStageCombined::setAxioms(const std::vector<std::vector<AxiomExprPlus> > &initial)
{
	mAxioms = initial;
}

const AxiomExprPlus &AXStageCombined::getAX(int aType, int n) const
{
	return mAxioms[aType][n];
}

bool AXStageCombined::isAXSelected(int aType, int n) const
{
	return true;
}

void AXStageCombined::setAXSelected(int aType, int n, bool value)
{
	// do nothing
}

int AXStageCombined::getAXSize() const
{
	return mAxioms.size();
}

int AXStageCombined::getAXSize(int aType) const
{
	return mAxioms[aType].size();
}

void AXStageCombined::recalculateMatterAxiomFunc(AxiomExprPlus &ax, int abType)
{
	if(!mComponents.empty()) {
		mComponents.front()->recalculateMatterAxiomFunc(ax, abType);
	}
}

void AXStageCombined::run()
{
	debug() << "AXStageCombined::run(): starting, number of components: " << mComponents.size();

	mAxioms.clear();
	unsigned total = 0;
	for(int i = 0; i < mComponents.size() ; ++i) {
		mComponents[i]->run();
		if(mAxioms.empty()) {
			mAxioms.resize(mComponents[i]->getAXSize());
		}
		for(unsigned aType = 0; aType < mComponents[i]->getAXSize(); ++aType) {
			for(int axiomNumber = 0; axiomNumber < mComponents[i]->getAXSize(aType); ++axiomNumber) {
				mAxioms[aType].push_back(mComponents[i]->getAX(aType, axiomNumber));
			}
			total += mComponents[i]->getAXSize(aType);
		}
	}

	debug() << "AXStageCombined finished. Total number of axioms: " << total;
}
