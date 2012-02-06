/****************************************************************************
*			SimpleTransmuteStrategy
*
*	Description:	Простая стратегия мутации
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Transmute_STRATEGY_SIMPLE_HXX
#define __AXIOMS_LIB_Transmute_STRATEGY_SIMPLE_HXX

#include "TransmuteStrategy.h"
#include <string>
#include "AxiomSetPop.h"

namespace AxiomLib {

class SimpleTransmuteStrategy : public TransmuteStrategy{

 private:
 
  double ectp;	// уровень мутации на уровне параметров элементарных условий
  double atp;	// уровень мутации на уровне аксиом
  double stp;	// уровень мутации на уровне системы аксиом
  double sptp;	// уровень мутации на уровне порядка аксиом в системе аксиом

    // Функция мутации отдельной системы аксиом - функция спратана в private для того, чтобы уменьшить число проверок внутренних параметров
  int transmuteAxiomSet (unsigned int asNum);

 protected:
  
 public:

  // Стандартный конструктор - пустой
  SimpleTransmuteStrategy (void) {ectp = 0.0; atp = 0.0; stp = 0.0; sptp = 0.0;}

  // Стандартный деструктор - пустой
  ~SimpleTransmuteStrategy () {}

  // Возвращает имя данного класса селекции
  std::string name (void) const {return (std::string)"TransmuteSimple";}

  // Функция задания значений параметров класса из окружения
  int setParamsFromEnv (const Environment& env);
  
  // Провести операцию селекции над данной популяцией
  int transmute (AxiomSetPop& pop);
  
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_Transmute_STRATEGY_HXX */
