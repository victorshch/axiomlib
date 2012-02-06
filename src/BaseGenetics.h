/****************************************************************************
*				BaseGenetics
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

class BaseGenetics {

private:
  // Базовые числовые параметры
  signed int numOfIter; // количество итераций генетического алгоритма (задающийся параметр)
  
  // Популяция
  AxiomSetPop axiomSetPop;

  // Стратегии работы алгоритма, инициализирующиеся из конфигурационного файла
  SelectionStrategy* selectionStrategy;
  TransmuteStrategy* transmuteStrategy;
  CrossoverStrategy* crossoverStrategy;
  GoalStrategy* goalStrategy;

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
  BaseGenetics (void);

  // Деструктор - удаляются все созданные в рамках данного класса динамические объекты
  ~BaseGenetics(void);
  
  // установка параметров генетического алгоритма
  // он сам выставляет нужные ему параметры
  int initFromEnv(const Environment& env);
  
  // усеченная установка параметров генетического алгоритма - устанавливаются не все параметры - так скажем нет чтения набора данных
  // он сам выставляет нужные ему параметры
  int shortInitFromEnv(const Environment& env);
  
  // Установка нового набора данных для данного класса подбора системы аксиом
  int setDataSet (const DataSet &newDataSet);
  
  // Установка нового значения числа итераций при подборе оптимальной системы аксиом
  int setNumOfIter (const int newNumOfIter);
  
  // Установка вектора указания канонического порядка параметров набора данных по которым ведется анализ
  int setParamsOfDataSet (std::vector<int> &params);

  // запуск генетического алгоритма
  int run(void);
  
  // запуск генетического алгоритма с записью получившейся популяции и возвратом наилучшего значения целевой функции
  double run (AxiomSetPop &asp);
  
  // вывод результатов
  int writeResultsToDisplay(void);

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_BASE_GENETICS_HXX */
