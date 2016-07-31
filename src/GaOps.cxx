/****************************************************************************
*
*			Функции класса GaOps
*
****************************************************************************/

//#include "AxiomSetWithStats.h"
#include "GaOps.h"
//#include "AxiomSet.h"

using namespace AxiomLib;
using namespace std;
namespace AxiomLib {

/****************************************************************************
*			CrossoverOp_Params
*
*	Description:	Скрещивание на уровне параметров элементарных условий
*	Parameters:	AxiomSetWithStats& result - результат скрещивания
*				const AxiomSet& first - первый операнд
*				const AxiomSet& second - второй операнд
*	Returns:		0
*	Throws:		AxiomLibException - если конфигурация некорректна
*	Author:		gevor
*	History:
*
****************************************************************************/
signed int CrossoverOp_Params(AxiomSetWithStats& result, const AxiomSet& first, const AxiomSet& second)
{
  // Проверяем структурную идентичность

  // обнулем статистику для результата
  result.voidStats();
  return 0;
}

}; // rnd of namespace
