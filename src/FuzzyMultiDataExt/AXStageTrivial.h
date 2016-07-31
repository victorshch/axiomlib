#pragma once 

#ifndef AXSTAGETRIVIAL_H
#define AXSTAGETRIVIAL_H

#include "AXStage.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class AXStageTrivial : public AXStage {
public: 
	AXStageTrivial(FuzzyDataSet *fuzzyDataSet, ECStage *stage1);

	void initFromEnv(const Environment& env) {}

		// Функции доступа к набору аксиом
	void setAxioms(const std::vector<std::vector<AxiomExprPlus> > &initial);

	const AxiomExprPlus &getAX(int aType, int n) const;
	bool isAXSelected(int aType, int n) const;
	void setAXSelected(int aType, int n, bool value);
	void getAXSize(std::vector<int> &result) const;
	int getAXSize() const;
	int getAXSize(int aType) const;

	void recalculateMatterAxiomFunc(AxiomExprPlus& ax, int abType) { }

	void run();

protected: 	
	AXMultiVector axioms;

	Logger* logger;
};

};

};
#endif