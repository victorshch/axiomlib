/****************************************************************************
*				GoalStrategyFactory
*
*	Description:	Фабричный метод создания стратегий вычисления цели
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Goal_STRATEGY_FACTORY_HXX
#define __AXIOMS_LIB_Goal_STRATEGY_FACTORY_HXX

#include "GoalStrategy.h"
#include <string>

namespace AxiomLib {

class GoalStrategyFactory {

 private:

 protected:
 
 public:

  GoalStrategyFactory(void) {};
  
  ~GoalStrategyFactory(void) {};
  
  GoalStrategy* create (const std::string name);
};

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_Goal_STRATEGY_FACTORY_HXX */
