/****************************************************************************
*			SimpleSelectionStrategy
*
*	Description:	Простая стратегия селекции
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_SELECTION_STRATEGY_SIMPLE_HXX
#define __AXIOMS_LIB_SELECTION_STRATEGY_SIMPLE_HXX

#include "SelectionStrategy.h"
#include "AxiomSetPop.h"
#include <string>
#include <stdlib.h>

namespace AxiomLib {

class SimpleSelectionStrategy : public SelectionStrategy{

 private:
  
  // Вспомогательная функция сортировки индексов mask по значениям goal
  inline int sort (std::vector<int> &mask, std::vector<double> &goals) const;
  
  // Аналогичная вспомогательная функция сортировки индексов mask по значениям goal
  inline int quickSort (std::vector<int> &mask, std::vector<double> &goals) const;

 protected:

  double pBest; // процент лучших
  
  bool comments; //  вывод комментариев в стандартный потох вывода

 public:

  // Стандартный конструктор - пустой
  SimpleSelectionStrategy (void) {pBest = -1;}

  // Стандартный деструктор - пустой
  ~SimpleSelectionStrategy () {}
  
  // Установка значения внетренней переменной comments - для контроля вывода информации в std::cout
  void setComments (const bool newComments) {comments = newComments;}

  // Возвращает имя данного класса селекции
  std::string name (void) const {return (std::string)"SelectionSimple";}

  // Функция задания значений параметров класса
  int setParamsFromEnv (const Environment& env);

  // Провести операцию селекции над данной популяцией
  int select (AxiomSetPop& pop) const;
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_SELECTION_STRATEGY_HXX */
