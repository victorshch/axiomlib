/****************************************************************************
*					checkName
*
*	Description:	Функция сравнения строк
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_CHECK_NAME_HXX
#define __AXIOMS_LIB_CHECK_NAME_HXX

#include <stdlib.h>
#include <string>

namespace AxiomLib {

	// Функция сравнения строк
	bool checkName (std::string name1, signed int from, signed int upTo, std::string name2);

}; // end of namespace

#endif // __AXIOMS_LIB_CHECK_NAME_HXX
