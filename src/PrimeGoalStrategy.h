/****************************************************************************
*			PrimeGoalStrategy
*
*	Description:	Простая стратегия подсчета целевой функции
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Goal_STRATEGY_Prime_HXX
#define __AXIOMS_LIB_Goal_STRATEGY_Prime_HXX

#include "GoalStrategy.h"
#include <string>
#include "AxiomSetPop.h"

namespace AxiomLib {

class PrimeGoalStrategy : public GoalStrategy{

 private:

 protected:

  // к-ты при ошибках I и II рода
  double gcFirst;
  double gcSecond;

 public:

  // Стандартный конструктор - пустой
  PrimeGoalStrategy (void) {gcFirst = -1; gcSecond=-1;};

  // Стандартный деструктор - пустой
  ~PrimeGoalStrategy () {};

  // Возвращает имя данного класса селекции
  std::string name (void) const {return (std::string)"GoalFunctionPrime";};

  // Функция задания значений параметров класса
  int setParamsFromEnv (const Environment& env);

  // Операция вычисления целевой функции для всех элементов популяции
  int compute (AxiomSetPop& pop) const;

  // Операция вычисления целевой функции по числу ошибок первого и второго рода
  double compute (int nFirst, int nSecond) const;

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_Goal_STRATEGY_HXX */
