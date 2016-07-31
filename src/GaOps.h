/****************************************************************************
*				GaOps
*
*	Description:	Класс генетических операций
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_GaOps_HXX
#define __AXIOMS_LIB_GaOps_HXX

#include "AxiomSetWithStats.h"
#include "AxiomSet.h"

namespace AxiomLib {

// Операция скрещивания двух систем аксиом на уровне параметров элементарных условий
signed int CrossoverOp_Params(AxiomSetWithStats& result, const AxiomSet& first, const AxiomSet& second);

}; //  end of namespace

#endif /* __AXIOMS_LIB_GaOps_HXX */
