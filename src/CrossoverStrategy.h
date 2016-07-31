/****************************************************************************
*			CrossoverStrategy
*
*	Description:	Суперкласс операции скрещивания
*	Author:			gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Crossover_STRATEGY_HXX
#define __AXIOMS_LIB_Crossover_STRATEGY_HXX

#include "Environment.h"
#include "AxiomSetPop.h"
#include "AxiomSetWithStats.h"
#include <string>
#include <vector>

namespace AxiomLib {

class CrossoverStrategy {

 private:

 protected:

  std::vector<AxiomSetWithStats>* asPop;

  std::vector<std::vector<Axiom *>* > axiomSets;
 
 public:

  // Конструктор класса с заданием значений по умолчанию
  CrossoverStrategy (void) {
	  asPop = NULL;
	  axiomSets.resize(0);
  }
  
  // Функция инициализации класса стратегии скрещивания
  // Получение вектора ссылок на наборы аксиом в каждом из asPop
  int initCrossover (AxiomSetPop &axiomSetPop) {
	  asPop = &(axiomSetPop.asPop);
	  axiomSets.resize(asPop->size());
	  for (unsigned int i = 0; i < asPop->size(); i++) {
		  axiomSets[i] = &((*asPop)[i].axioms);
	  }
	  return 0;
  }

  // Деструктор класса
  virtual ~CrossoverStrategy () {}

  // Возвращает имя данного класса селекции
  virtual std::string name (void) const = 0;

  // Функция задания значений параметров класса
  virtual int setParamsFromEnv (const Environment& env) = 0;
  virtual int setParams (const double newCrossLevel) = 0;
  virtual int setParams (const double newElemCondCrossLevel, const double newAxiomCrossLevel, const double newAxiomSetCrossLevel) = 0;

  // Провести операцию селекции над данной популяцией (при этом популяция меняется, вообще говоря - удваивается)
  virtual int crossover (AxiomSetPop& pop) = 0;
  
}; // end of class

struct Pair {
	unsigned short first;
	unsigned short second;
};

struct Quad {
	unsigned short first;
	unsigned short fax;
	unsigned short second;
	unsigned short sax;
};

struct QuadPlus {
	unsigned short first;
	unsigned short fax;
	unsigned short second;
	unsigned short sax;
	std::string ecName;
};

}; //  end of namespace

#endif /* __AXIOMS_LIB_Crossover_STRATEGY_HXX */
