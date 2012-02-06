/****************************************************************************
*					AxiomSetStructure
*
*	Description:	Класс, содержащий описание структуры системы аксиом
*	Author:			dk
*	History:     	-
*
****************************************************************************/
#ifndef __qtGui_AxiomSetStructure_HXX
#define __qtGui_AxiomSetStructure_HXX

#include "AxiomStructure.h"

class AxiomSetStructure {

public:
	
	// Название системы аксиом
	std::string axiomSetName;
	// Название банка аксиом из которого могут быть взяты аксиомы
	std::string axiomBankDir;
	// Набор аксиом, составляющих систему аксиом
	std::vector <AxiomStructure> axioms;
	
}; //end of class

#endif /* __qtGui_AxiomSetStructure_HXX */
