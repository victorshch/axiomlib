/****************************************************************************
*			all_axioms
*
*	Description:	"Супер-include", содержащий все includes частных аксиом
*			генерируется автоматически (not anymore =\)
*	Author:		Eugene A. Vasin
*	History:
*
****************************************************************************/
/*!
  @file all_axioms.h
  @author Eugene A. Vasin
  Super-include, which contains each includes of particular axioms
  Generate automatically(not anymore =\)
 */

// BEGIN AXIOM HEADERS
#include "elem_conditions/ElemCondition.h"
#include "elem_conditions/DecreaseRightElemCondition.h"
#include "elem_conditions/SecDerivativeDecreaseLeft.h"
#include "elem_conditions/DerivativeDecreaseLeft.h"
#include "elem_conditions/BetweenTwoLevels.h"
#include "elem_conditions/Volatility.h"
#include "elem_conditions/SecDerivativeIncreaseLeft.h"
#include "elem_conditions/SecDerivativeDecreaseRight.h"
#include "elem_conditions/DerivativeDecreaseRight.h"
#include "elem_conditions/DerivativeIncreaseRight.h"
#include "elem_conditions/DerivativeIncreaseLeft.h"
#include "elem_conditions/Integral.h"
#include "elem_conditions/SecDerivativeIncreaseRight.h"
#include "elem_conditions/DecreaseLeftElemCondition.h"
#include "elem_conditions/IncreaseRightElemCondition.h"
#include "elem_conditions/IncreaseLeftElemCondition.h"
#include "elem_conditions/Variance.h"

// TODO refactor
/*!
 @def N_EL_AXIOMS
 N_EL_AXIOMS = 16
*/
#define N_EL_AXIOMS 16
// END AXIOM HEADERS
