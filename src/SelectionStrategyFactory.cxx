/****************************************************************************
*
*			Функции класса SelectionStrategyFactory
*
****************************************************************************/

#include "SelectionStrategyFactory.h"
#include "SimpleSelectionStrategy.h"
#include "AxiomLibException.h"

using namespace AxiomLib;
using namespace std;

/****************************************************************************
*			SelectionStrategyFactory::create
*
*	Description:	Создает класс селекции
*	Parameters:	name - имя класса, экземпляр которого создаем
*	Returns:	Selection* - указатель на созданный класс
*	Throws:		AxiomLibException - если класса нет
*	Author:		dk
*	History:
*
****************************************************************************/
SelectionStrategy* SelectionStrategyFactory::create (const std::string name) {
  if (name == "SelectionSimple") {
    SimpleSelectionStrategy* toReturn;
    toReturn = new SimpleSelectionStrategy;
    return toReturn;
  }
  throw AxiomLibException ("SelectionStrategyFactory::create : can not find the requested selection strategy.");
}
