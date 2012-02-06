/****************************************************************************
*			SimpleCrossoverStrategy
*
*	Description:	Простая стратегия скрещивания: только на уровне параметров
*			элементарных условий
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Crossover_STRATEGY_SIMPLE_HXX
#define __AXIOMS_LIB_Crossover_STRATEGY_SIMPLE_HXX

#include "CrossoverStrategy.h"
#include <string>
#include "AxiomSetPop.h"

namespace AxiomLib {

class SimpleCrossoverStrategy : public CrossoverStrategy{

 private:

 protected:

  double pCross; // процент участвующих

 public:

  // Стандартный конструктор - пустой
  SimpleCrossoverStrategy (void) {pCross = -1;};

  // Стандартный деструктор - пустой
  ~SimpleCrossoverStrategy () {};

  // Возвращает имя данного класса селекции
  std::string name (void) const {return (std::string)"CrossoverSimple";};

  // Функция задания значений параметров класса
  int setParamsFromEnv (const Environment& env);
  int setParams (const double newCrossLevel);
  int setParams (const double newElemCondCrossLevel, const double newAxiomCrossLevel, const double newAxiomSetCrossLevel);

  // Провести операцию селекции над данной популяцией
  int crossover (AxiomSetPop& pop);
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_Crossover_STRATEGY_HXX */
