/****************************************************************************
*						AxiomExprSetStructure
*
*	Description:	Описание структуры системы аксиом
*	Author:			dk
*	History:     	-
*
****************************************************************************/

#ifndef __AXIOMS_LIB_AxiomExprSetStructure_HXX
#define __AXIOMS_LIB_AxiomExprSetStructure_HXX

#include "AxiomExprStructure.h"
#include <vector>
#include <string>

namespace AxiomLib {

class AxiomExprSetStructure {

public:
	
	// Название данной системы аксиом
	std::string axiomSetName;
	// Название используемого банка аксиом
	std::string axiomBankDir;
	// Описание аксиом, составляющих данную систему аксиом
	std::vector <AxiomExprStructure> axioms;
	
}; //end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_AxiomExprSetStructure_HXX */
