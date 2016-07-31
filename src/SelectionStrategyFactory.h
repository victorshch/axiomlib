/****************************************************************************
*				SelectionStrategyFactory
*
*	Description:	Фабричный метод создания стратегий селекции
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_SELECTION_STRATEGY_FACTORY_HXX
#define __AXIOMS_LIB_SELECTION_STRATEGY_FACTORY_HXX

#include "SelectionStrategy.h"
#include <string>

namespace AxiomLib {

class SelectionStrategyFactory {

 private:

 protected:
 
 public:

  SelectionStrategyFactory(void) {};
  
  ~SelectionStrategyFactory(void) {};
  
  SelectionStrategy* create (const std::string name);
};

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_SELECTION_STRATEGY_FACTORY_HXX */
