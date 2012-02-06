/****************************************************************************
*				ECTypeStage
*
*	Description: Класс, соответствующий стадии задания типов ЭУ
*				алгоритма FuzzyMultiDataExt
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef FUZZYMULTIDATAEXT_ECTYPE_H
#define FUZZYMULTIDATAEXT_ECTYPE_H

#include "../Environment.h"

#include "Common.h"
#include "../Logger.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

typedef ElementSelection<AxiomLib::ElemCondPlus> ECTypeSelection;
	
class ECTypeStage
{
public:
    ECTypeStage();
	
	void setECTypes(const std::vector<ElemCondPlus> &value);
	
	void setLogger(Logger *logger);
	void initFromEnv(const Environment &env);
	
	// Функции доступа к набору типов ЭУ
	const ElemCondPlus &getECType(int n) const;
	bool isECTypeSelected(int n) const;
	void setECTypeSelected(int n, bool value);
	int getECTypeSize() const;
	
//	template<class Archive>
//	void serialize(Archive &archive) const {
//		FuzzyMultiDataExt_ECType *self = const_cast<FuzzyMultiDataExt_ECType *>(this);
		
//		archive & self->ecTypes;
//	}
	
private:
	
	std::vector<ECTypeSelection> ecTypes;
	
	Logger* logger;
};

};

};

#endif // FUZZYMULTIDATAEXT_ECTYPE_H
