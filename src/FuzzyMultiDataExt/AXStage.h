/****************************************************************************
*				AXStage
*
*	Description: Класс, соответствующий стадии построения аксиом
*				алгоритма FuzzyMultiDataExt
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef FUZZYMULTIDATAEXT_AX_H
#define FUZZYMULTIDATAEXT_AX_H

#include <boost/tuple/tuple.hpp>

#include "../Environment.h"
#include "../FuzzyDataSet.h"
#include "../Logger.h"
#include "../SatPointSet.h"

#include "Common.h"
#include "ForwardDeclarations.h"
#include "ReducedSatPointSet.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

typedef ElementSelection<AxiomLib::AxiomExprPlus> AXSelection;
	
class AXStage
{
public:
	static AXStage* create(std::string name, FuzzyDataSet* fuzzyDataSet,
				  ECStage* stage1);

	AXStage(FuzzyDataSet* fuzzyDataSet,
	                     ECStage* stage1);
	
	virtual void initFromEnv(const Environment& env) = 0;
	
	// Функции доступа к набору аксиом
	virtual void setAxioms(const std::vector<std::vector<AxiomExprPlus> > &initial) = 0;
	
	virtual const AxiomExprPlus &getAX(int aType, int n) const = 0;
	virtual bool isAXSelected(int aType, int n) const = 0;
	virtual void setAXSelected(int aType, int n, bool value) = 0;
	virtual int getAXSize() const = 0;
	virtual int getAXSize(int aType) const = 0;
	
	virtual void recalculateMatterAxiomFunc(AxiomExprPlus& ax, int abType) = 0;
	
	virtual void run() = 0;

	FuzzyMultiDataExtAlgorithm* getParent() const { return parent; }
	void getAXSize(std::vector<int> &result) const;
	
protected:
	
	FuzzyMultiDataExtAlgorithm* parent;
		
	const FuzzyDataSet* fuzzyDataSet;
	const ECStage* stage1;	
};

};

};

#endif // FUZZYMULTIDATAEXT_AX_H
