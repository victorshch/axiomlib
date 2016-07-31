/****************************************************************************
*				ExtGenetics
*
*	Description:	Базовый класс генетического алгоритма оптимизации
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_BASE_GENETICS_HXX
#define __AXIOMS_LIB_BASE_GENETICS_HXX

#include "Environment.h"
#include "AxiomSetPop.h"
#include "PopElemExt.h"
#include "AxiomSetWithStats.h"
#include "SelectionStrategy.h"
#include "TransmuteStrategy.h"
#include "CrossoverStrategy.h"
#include "SelectionStrategyFactory.h"
#include "TransmuteStrategyFactory.h"
#include "CrossoverStrategyFactory.h"
#include "GoalStrategyFactory.h"
#include "AxiomLibException.h"
#include "GoalStrategy.h"
#include "DataSet.h"
#include "Recognizer.h"
#include "RecognizerFactory.h"

namespace AxiomLib {

class ExtGenetics {

private:
  // Базовые числовые параметры
  signed int numOfIter; // количество итераций генетического алгоритма (задающийся параметр)
  
  // Популяция
  std::vector <PopElemExt> population;
  // Initial population size (number from environment)
  int popSize;
  
  // Вторая часть популяции
  std::vector <double> paramsPop;

  // Стратегии работы алгоритма, инициализирующиеся из конфигурационного файла
  //SelectionStrategy* selectionStrategy;
  //TransmuteStrategy* transmuteStrategy;
  CrossoverStrategy* crossoverStrategy;
  GoalStrategy* goalStrategy;
  
  // Degree of crossover at level of elementary conditions, at the level of axioms and at the level of axiom set
  double eccl;
  double acl;
  double ascl; 

  // Degree of tranmsute at level of elementary conditions and at the level of axioms
  double ectp;
  double atp;
  // Tranmsute degree of precision parameter
  double prtp;

  // Класс распознавателя
  Recognizer* recognizer;
  
  // Переменная для хранения значения целевой функции, по которому возможно досрочное завершение генетического алгоритма
  double exitCond;
  
  // Путь для сохранения лучшей системы аксиом по результатам работы цикла
  std::string *axiomSetBaseDir; // директория для сохранения
  std::string *whereToSave; //имя файла дял сохранения
  
  // Флаг, отвечающий за вывод технической информации в стандартный поток вывода (устанавливаетя при создании класса или по умочанию)
  bool comments;
  
protected:
 
public:

  // Пустой конструктор с заданием параметров по умолчанию
  ExtGenetics (void);

  // Деструктор - удаляются все созданные в рамках данного класса динамические объекты
  ~ExtGenetics(void);
  
  // установка параметров генетического алгоритма
  // он сам выставляет нужные ему параметры
  int initFromEnv(const Environment& env);
  
  // Установка нового значения числа итераций при подборе оптимальной системы аксиом
  int setNumOfIter (const int newNumOfIter);

  // запуск генетического алгоритма
  int run(void);
  
  int select ();

  int transmute (); 
  
  int crossover ();

  int storeBest (PopElemExt &bestElem);

  void writeResultsToDisplay (void);

  int packAxiomSetPop (AxiomSetPop &aspTemp);

  int packAxiomSetPop (AxiomSetPop &aspTemp, std::vector<double> &prec);

  int unpackAxiomSetPop (AxiomSetPop &aspTemp);

  int unpackAxiomSetPop (AxiomSetPop &aspTemp, std::vector<double> &prec);

  inline int sort (std::vector<int> &mask, std::vector<double> &goals) const;

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_BASE_GENETICS_HXX */
