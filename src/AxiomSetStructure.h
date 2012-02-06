/****************************************************************************
*						AxiomSetStructure
*
*	Description:	-
*	Author:		dk
*	History:     	-
*
****************************************************************************/

#ifndef __AXIOMS_LIB_AxiomSetStructure_HXX
#define __AXIOMS_LIB_AxiomSetStructure_HXX

#include "AxiomStructure.h"

namespace AxiomLib {

class AxiomSetStructure {

public:
	
	std::string axiomSetName;
	
	std::string axiomBankDir;
	
	std::vector <AxiomStructure> axioms;
	
	
}; //end of class

typedef std::map <std::string, AxiomSetStructure> AxiomSets;

}; //  end of namespace

#endif /* __AXIOMS_LIB_AxiomSetStructure_HXX */
