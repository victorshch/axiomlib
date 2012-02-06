/****************************************************************************
*			TransmutePrecision
*
*	Description:	Простая стратегия селекции
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Transmute_STRATEGY_Precision_HXX
#define __AXIOMS_LIB_Transmute_STRATEGY_Precision_HXX

#include "TransmuteStrategy.h"
#include <string>
#include "AxiomSetPop.h"

namespace AxiomLib {

class TransmutePrecision : public TransmuteStrategy{

 private:

 protected:

  double ecTransLevel; // уровень мутации на уровне параметров Ec

 public:

  // Стандартный конструктор - пустой
  TransmutePrecision (void) {ecTransLevel = -1;}

  // Стандартный деструктор - пустой
  ~TransmutePrecision () {}

  // Возвращает имя данного класса селекции
  std::string name (void) const {return (std::string)"TransmuteSimple";}

  // Функция задания значений параметров класса
  int setParamsFromEnv (const Environment& env);

  // Провести операцию селекции над данной популяцией
  int transmute (AxiomSetPop& pop);
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_Transmute_Precision_HXX */
