/****************************************************************************
*				AxiomSetPop
*
*	Description:	Популяция с особями - наборами аксиом
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_AXIOM_SET_POP_HXX
#define __AXIOMS_LIB_AXIOM_SET_POP_HXX

#include "AxiomSetWithStats.h"
#include "Environment.h"
#include <vector>
#include <set>

namespace AxiomLib {

class AxiomSetPop {

private:

  friend class CrossoverStrategy;
  
  friend class TransmuteStrategy;

protected:

  std::vector<AxiomSetWithStats>  asPop; // популяция - axiom sets
  
  int popSize; // размер популяции, как он указан в конфиге
  
public:

  // Пустой конструктор с заданием параметров по умолчанию
  AxiomSetPop (void) {popSize = -1; /*asPop = new std::vector<AxiomSetWithStats>;*/ asPop.resize(0);}

  // Деструктор - удаляются все созданные в рамках данного класса динамические объекты
  ~AxiomSetPop(void) {}
  
  // Инициализация начальной популяции систем аксиом из окружения
  int initFromEnv (const Environment& env);
    
  // Добавляет сгенерированные систмы аксиом в популцию
  void clear ();
  
  // Добавляет сгенерированные систмы аксиом в популцию
  void addSets (std::vector < std::vector<AxiomSetWithStats> > &newSets, std::vector < std::vector <int> > &setsMask);
  // Добавляет сгенерированные систмы аксиом в популцию
  void addSets (std::vector<AxiomSetWithStats>  &newSets); 
  
  // сброс статистики системы аксиом под номером asNum в векторе asPop
  void voidStats (const int asNum);
  // установка статистики системы аксиом под номером asNum в векторе asPop
  void setStats (const int asNum, signed int nf, signed int ns, double g);
  // получение статистики, без проверок, для системы аксиом под номером asNum в векторе asPop
  void getStats (const int asNum, signed int& nf, signed int& ns, double& g) const;
  // copy axiom set with statistic by number asNum from this class
  AxiomSetWithStats& copyAxiomSet (const int asNum);
  
  // функция проверяет - существует ли система аксиом с целевой функцией ниже заданного значения, попутно выбираем лучшую систему аксиом
  bool exit (double exitCond, AxiomSetWithStats &bestAxiomSet);
  
  // функция прореживания популяции - удляются из популяции все особи - по индексу которых стоит false
  int cutDown (std::vector <bool> &stayAlive);
  
  // разметить ряд row начиная с индеска begin заканичвая индексом end системой аксиом номер asNum из вектора asPop и записать результаты разметки в ряд markup
  int enter (std::vector<int>& markup, const std::vector<double>& row, const unsigned long begin, const unsigned long end, int asNum);
  
  // получить базовый размер набора аксиом (из конфига)
  int getBasePopSize(void) const;
  // получить текущий размер популяции
  int size(void) const;

  // Функция возвращающая имя системы аксиом по номеру в популяции
  std::string name (int index) const;

  // сохранение систем аксиом в файлы
  int saveAxiomSetPopToFiles (Environment &env);
  
  // Оператор копирования популяции
  //AxiomSetPop & operator= (AxiomSetPop& second);

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_BASE_GENETICS_HXX */
