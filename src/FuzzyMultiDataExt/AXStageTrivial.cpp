#include "AXStageTrivial.h"

#include "../AxiomStatistics.h"
#include "../FuzzyMultiDataExtAlgorithm.h"
#include "ECStage.h"

using namespace std;
using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

AXStageTrivial::AXStageTrivial(FuzzyDataSet *fuzzyDataSet, ECStage *stage1)
	:AXStage(fuzzyDataSet, stage1), logger(Logger::getInstance()){}

AXSelection axSelectionCreator1(const AxiomExprPlus& ax) { return AXSelection(ax); }

void AXStageTrivial::setAxioms(const std::vector<std::vector<AxiomExprPlus> > &initial) {
	TransformMultiVectorHelper<2>::transform(initial, this->axioms,
												axSelectionCreator1
	);
}

const AxiomExprPlus &AXStageTrivial::getAX(int aType, int n) const {
	return axioms[aType][n].element();
}

bool AXStageTrivial::isAXSelected(int aType, int n) const {
	return axioms[aType][n].selected();
}

void AXStageTrivial::setAXSelected(int aType, int n, bool value) {
	axioms[aType][n].setSelected(value);
}

void AXStageTrivial::getAXSize(std::vector<int> &result) const {
	result.clear();
	result.reserve(axioms.size());

	for(auto it = axioms.begin(); it != axioms.end(); ++it)
	{
		const std::vector<AXSelection>& v = *it;
		result.push_back(v.size());
	}
}

int AXStageTrivial::getAXSize() const {
	return axioms.size();
}

int AXStageTrivial::getAXSize(int aType) const {
	return axioms.at(aType).size();
}

void AXStageTrivial::run(){
	this->axioms.resize(this->stage1->getECSize());

	for (int i = 0; i  < this->stage1->getECSize(); ++i) {

		vector<AXSelection> axSelection;

		for (int j = 0; j < this->stage1->getECSize(i); ++j) {
			for (int k = 0; k  < this->stage1->getECSize(i, j); ++k) {
				for (int l = 0; l < this->stage1->getECSize(i,j,k); ++l){
					const AxiomExprPlus exp = AxiomExprPlus(this->stage1->getEC(i, j, k, l));
					axSelection.push_back(AXSelection(exp, true));
				}
			}
		}

		this->axioms.push_back(axSelection);
	}	
}