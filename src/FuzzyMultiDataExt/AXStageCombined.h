#ifndef AXSTAGECOMBINED_H
#define AXSTAGECOMBINED_H

#include "AXStage.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class AXStageCombined : public AXStage
{
public:
	AXStageCombined(FuzzyDataSet* fuzzyDataSet,
					ECStage* stage1) : AXStage(fuzzyDataSet, stage1) {}

	~AXStageCombined();

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

	FuzzyMultiDataExtAlgorithm* getParent() const { return parent; }
	void getAXSize(std::vector<int> &result) const;

private:
	std::vector<AXStage*> mComponents;
	std::vector<std::vector<AxiomExprPlus> > mAxioms;
};

}

}

#endif // AXSTAGECOMBINED_H
