/****************************************************************************
*			AxiomSetwithStats
*
*	Description:	Система аксиом + статистика по ней
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_AXIOM_SET_WITH_STATS_HXX
#define __AXIOMS_LIB_AXIOM_SET_WITH_STATS_HXX

#include "AxiomSet.h"
#include "AxiomBase.h"
#include <string>

namespace AxiomLib {

class AxiomSetWithStats:public AxiomSet {

 private:

 protected:
 
  double goal; // значение целевой функции
  int nFirst;  // кол-во ошибок первого рода
  int nSecond; // кол-во ошибок второго рода

 public:

  // Установка другом класс общего родителя классов стратегии скрещивания
  friend class CrossoverStrategy;
  
  // Установка другом класс общего родителя классов стратегии мутации
  friend class TransmuteStrategy;
 
  // Пустой конструктор с заданием имени по default'у
  AxiomSetWithStats(void) : AxiomSet () {nFirst = -1; nSecond = -1; goal = -1;}
  
  // Пустой конструктор с заданием имени в параметре
  AxiomSetWithStats(const std::string sName) : AxiomSet (sName) {nFirst = -1; nSecond = -1; goal = -1;}
	 
  // Деструктор удаляющий все созданные в рамках данной системы аксиом динамические объекты
  ~AxiomSetWithStats(void) {};
  
  // сравнение по goal
  bool operator< (const AxiomSetWithStats& second) const;
  
  // сброс статистики
  inline void voidStats(void) {nFirst = -1; nSecond = -1; goal = -1;}
  // установка статистики
  inline void setStats(signed int nf, signed int ns, double g) {nFirst = nf; nSecond = ns; goal = g;}
  // получение статистики, без проверок
  inline void unsafeGetStats(signed int& nf, signed int& ns, double& g) const {nf = nFirst; ns = nSecond; g = goal;}
  
//   // проверка на соответствие значения целевой функции
//   bool exit (double exitCond);
  
  // инициализация набора аксиом из файла
  signed int initAxiomSetFromFile (std::string axiomSetBaseDir, std::string axiomSetName, AxiomBase &axiomBase);
  
  // вывод набора аксиом на дисплей
  int outputToCout (void) const;
  
  // копирует goal, nFirst и nSecond
  AxiomSetWithStats & operator= (const AxiomSetWithStats &second);
    
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_AXIOM_SET_WITH_STATS_HXX */
