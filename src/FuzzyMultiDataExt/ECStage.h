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
	static ECStage* create(const std::string& name, FuzzyDataSet* fuzzyDataSet, ECTypeStage* stage0);

	virtual void setECs(const std::vector<std::vector<std::vector<std::vector<ElemCondPlusStat> > > > & value) {
		// TODO:
	}
	
	virtual int initFromEnv(const Environment& env) = 0;
	
	virtual void run() = 0;
	
	// Функции доступа к набору ЭУ
	virtual const ElemCondPlusStat & getEC(int aType, int dimension, int type, int n) const;

	virtual int getECSize() const ;
	virtual int getECSize(int aType) const ;
	virtual int getECSize(int aType, int dimension) const;
	virtual int getECSize(int aType, int dimension, int ecType) const;
	virtual void getECSize(std::vector<std::vector<std::vector<int> > > &result) const ;
	
	virtual int getECTotalCount() const;

	virtual bool isECSelected(int aType, int dimension, int type, int n) const { return true; }
	
	virtual void setECSelected(int aType, int dimension, int type, int n, bool value) {}

	virtual void recalculateMatterECFunc(ElemCondPlusStat& ec, int abType) const {}	

	FuzzyMultiDataExtAlgorithm* getParent() const { return parent; }
protected:

	void checkIndices(int i, int j, int k, int l) const;

	typedef std::vector<std::vector<std::vector<std::vector<
		ECSelection
		> > > > ECMultiVector;

	ECMultiVector elemConditions;	
	
	const FuzzyDataSet* fuzzyDataSet;
	const ECTypeStage* stage0;

	FuzzyMultiDataExtAlgorithm* parent;
};

};

};

#endif // FUZZYMULTIDATAEXT_EC_H
