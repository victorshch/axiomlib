/****************************************************************************
*				ASStage
*
*	Description: Класс, соответствующий стадии построения системы аксиом
*				алгоритма FuzzyMultiDataExt
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef ASSTAGE_H
#define ASSTAGE_H

#include <string>

#include "../ReducedRecognizer.h"
#include "../GoalStrategy.h"

#include "../Environment.h"

#include "../FuzzyDataSet.h"

#include "Common.h"
#include "ForwardDeclarations.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class ASStage
{
public:
	static ASStage* create(const std::string& name, FuzzyDataSet* fuzzyDataSet, 
						   AXStage* stage2);
	
	virtual void initFromEnv(const Environment& env) = 0;
	
	virtual void run() = 0;
	
	virtual AxiomExprSetPlus &getAS(int n) = 0;
	virtual const AxiomExprSetPlus &getAS(int n) const = 0;
	virtual int getASSize() const = 0;
	virtual void recalculateMatterASFunc(AxiomExprSetPlus& as) = 0;
	virtual void setAxiomSets(const std::vector<AxiomExprSetPlus>& initialAS) = 0;
};

};

};

#endif // ASSTAGE_H
