/****************************************************************************
*
*			Функции класса GoalStrategyFactory
*
****************************************************************************/

#include "GoalStrategyFactory.h"
#include "PrimeGoalStrategy.h"
#include "AxiomLibException.h"

using namespace AxiomLib;
using namespace std;

/****************************************************************************
*			GoalStrategyFactory::create
*
*	Description:	Создает класс вычисления цели
*	Parameters:	name - имя класса, экземпляр которого создаем
*	Returns:	Goal* - указатель на созданный класс
*	Throws:		AxiomLibException - если класса нет
*	Author:		dk
*	History:
*
****************************************************************************/
GoalStrategy* GoalStrategyFactory::create (const std::string name) {
  if (name == "GoalFunctionPrime") {
    PrimeGoalStrategy* toReturn;
    toReturn = new PrimeGoalStrategy;
    return toReturn;
  }
  throw AxiomLibException ("GoalStrategyFactory::create : can not find the requested Goal strategy.");
}
