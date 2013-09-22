/****************************************************************************
*				ECStage
*
*	Description: Класс, соответствующий стадии построения ЭУ
*				алгоритма FuzzyMultiDataExt
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef FUZZYMULTIDATAEXT_EC_H
#define FUZZYMULTIDATAEXT_EC_H

#include <boost/tuple/tuple.hpp>

#include "../Environment.h"
#include "../FuzzyDataSet.h"
#include "Common.h"
#include "ForwardDeclarations.h"


namespace AxiomLib {

namespace FuzzyMultiDataExt {

typedef ElementSelection<AxiomLib::ElemCondPlusStat> ECSelection;
	
class ECStage
{
public:		
	virtual void setECs(const std::vector<std::vector<std::vector<std::vector<ElemCondPlusStat> > > > & value) = 0;
	
	virtual int initFromEnv(const Environment& env) = 0;
	
	virtual void run() = 0;
	
	// Функции доступа к набору ЭУ
	virtual const ElemCondPlusStat & getEC(int aType, int dimension, int type, int n) const = 0;
	
	virtual bool isECSelected(int aType, int dimension, int type, int n) const = 0;
	
	virtual void setECSelected(int aType, int dimension, int type, int n, bool value) = 0;
	
	virtual void getECSize(std::vector<std::vector<std::vector<int> > > &result) const = 0;
	
	virtual int getECSize() const  = 0;
	virtual int getECSize(int aType) const  = 0;
	virtual int getECSize(int aType, int dimension) const = 0;
	virtual int getECSize(int aType, int dimension, int ecType) const = 0;
	
	virtual int getECTotalCount() const = 0;
	
	virtual void recalculateMatterECFunc(ElemCondPlusStat& ec, int abType) const = 0;

	FuzzyMultiDataExtAlgorithm* getParent() const { return parent; }
	
protected:
	
	const FuzzyDataSet* fuzzyDataSet;
	const ECTypeStage* stage0;

	FuzzyMultiDataExtAlgorithm* parent;
};

};

};

#endif // FUZZYMULTIDATAEXT_EC_H
