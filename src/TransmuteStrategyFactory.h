/****************************************************************************
*				TransmuteStrategyFactory
*
*	Description:	Фабричный метод создания стратегий селекции
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Transmute_STRATEGY_FACTORY_HXX
#define __AXIOMS_LIB_Transmute_STRATEGY_FACTORY_HXX

#include "TransmuteStrategy.h"
#include "AllTransmute.h"
#include <string>

namespace AxiomLib {

class TransmuteStrategyFactory {

 private:

 protected:
 
 public:

  TransmuteStrategyFactory(void) {};
  
  ~TransmuteStrategyFactory(void) {};
  
  TransmuteStrategy* create (const std::string name);
};

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_Transmute_STRATEGY_FACTORY_HXX */
