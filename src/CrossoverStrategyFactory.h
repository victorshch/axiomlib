/****************************************************************************
*				CrossoverStrategyFactory
*
*	Description:	Фабричный метод создания стратегий селекции
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Crossover_STRATEGY_FACTORY_HXX
#define __AXIOMS_LIB_Crossover_STRATEGY_FACTORY_HXX

#include "CrossoverStrategy.h"
#include "AllCrossover.h"
#include "AxiomLibException.h"
#include <string>

namespace AxiomLib {

class CrossoverStrategyFactory {

 private:

 protected:
 
 public:

  CrossoverStrategyFactory(void) {};
  
  ~CrossoverStrategyFactory(void) {};
  
  CrossoverStrategy* create (const std::string name);
};

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_Crossover_STRATEGY_FACTORY_HXX */
