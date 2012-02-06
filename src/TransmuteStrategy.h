/****************************************************************************
*			TransmuteStrategy
*
*	Description:	Суперкласс операции селекции
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Transmute_STRATEGY_HXX
#define __AXIOMS_LIB_Transmute_STRATEGY_HXX

#include "Environment.h"
#include "AxiomSetPop.h"
#include <string>
#include <vector>

namespace AxiomLib {

class TransmuteStrategy {

 private:

 protected:
 
  std::vector<AxiomSetWithStats>*  asPop;

  std::vector<std::vector<Axiom *>* > axiomSets;
 
 public:

  // Стандартный конструктор - пустой
  TransmuteStrategy (void) {
	asPop = NULL;
	axiomSets.resize(0);
  }

  // Стандартный деструктор - пустой
  virtual ~TransmuteStrategy () {}

  // Возвращает имя данного класса селекции
  virtual std::string name (void) const = 0;

  // Функция задания значений параметров класса
  virtual int setParamsFromEnv (const Environment& env) = 0;

  // Провести операцию селекции над данной популяцией
  virtual int transmute (AxiomSetPop& pop) = 0;
  
  // Функция инициализации класса стратегии мутации
  // Получение вектора ссылок на наборы аксиом в каждом из asPop
  int initTransmute (AxiomSetPop &axiomSetPop) {
	asPop = &(axiomSetPop.asPop);
	axiomSets.resize(asPop->size());
	for (unsigned int i = 0; i < asPop->size(); i++) {
		axiomSets[i] = &((*asPop)[i].axioms);
	}
	return 0;
  }
  
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_Transmute_STRATEGY_HXX */
