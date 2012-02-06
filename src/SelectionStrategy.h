/****************************************************************************
*			SelectionStrategy
*
*	Description:	Суперкласс операции селекции
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_SELECTION_STRATEGY_HXX
#define __AXIOMS_LIB_SELECTION_STRATEGY_HXX

#include "Environment.h"
#include "AxiomSetPop.h"
#include <string>
#include <vector>

namespace AxiomLib {

class SelectionStrategy {

 private:

 protected:
 
 public:

  // Стандартный конструктор - пустой
  SelectionStrategy (void) {}

  // Стандартный деструктор - пустой
  virtual ~SelectionStrategy () {}

  // Возвращает имя данного класса селекции
  virtual std::string name (void) const = 0;

  // Устанавливает значение переменной контролирующей вывод в стандарный поток комментариев
  virtual void setComments (const bool newComments) = 0;
  
  // Функция задания значений параметров класса
  virtual int setParamsFromEnv (const Environment& env) = 0;

  // Провести операцию селекции над данной популяцией
  virtual int select (AxiomSetPop& pop) const = 0;
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_SELECTION_STRATEGY_HXX */
