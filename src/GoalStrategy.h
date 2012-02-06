/****************************************************************************
*			GoalStrategy
*
*	Description:	Суперкласс целевых функций (в зависимости от числа ошибок I и II рода)
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Goal_STRATEGY_HXX
#define __AXIOMS_LIB_Goal_STRATEGY_HXX

#include "Environment.h"
#include "AxiomSetPop.h"
#include <string>
#include <vector>

namespace AxiomLib {

class GoalStrategy {

 private:

 protected:
 
 public:

  // Стандартный конструктор - пустой
  GoalStrategy (void) {}

  // Стандартный деструктор - пустой
  virtual ~GoalStrategy () {}

  // Возвращает имя данного класса селекции
  virtual std::string name (void) const = 0;

  // Функция задания значений параметров класса
  virtual int setParamsFromEnv (const Environment& env) = 0;

  // Операция вычисления целевой функции для всех элементов популяции
  virtual int compute (AxiomSetPop& pop) const = 0;

  // Операция вычисления целевой функции по числу ошибок первого и второго рода
  virtual double compute (int nFirst, int nSecond) const = 0;

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_Goal_STRATEGY_HXX */
