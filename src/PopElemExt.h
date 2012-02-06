/****************************************************************************
*				
*
*	Description:	
*	Author:		
*	History:		
*
****************************************************************************/

#ifndef __AXIOMS_LIB_POP_ELEM_EXT_HXX
#define __AXIOMS_LIB_POP_ELEM_EXT_HXX

#include "AxiomSetWithStats.h"
#include "Environment.h"
#include <vector>
#include <set>

namespace AxiomLib {

class PopElemExt {

private:

protected:

public:

  AxiomSetWithStats axSet;
  
  double prec;
  
  // Пустой конструктор с заданием параметров по умолчанию
  PopElemExt (void) {}

  // Деструктор - удаляются все созданные в рамках данного класса динамические объекты
  ~PopElemExt (void) {}
  
  // Инициализация начальной популяции систем аксиом из окружения
  int initFromEnv (const Environment& env);
    
  // сброс статистики системы аксиом под номером asNum в векторе asPop
  void voidStats ();
  
  // установка статистики
  void setStats (signed int nf, signed int ns, double g);
  
  // получение статистики, без проверок
  void getStats (signed int& nf, signed int& ns, double& g) const;
  
  // разметить ряд row начиная с индеска begin заканичвая индексом end системой аксиом номер asNum из вектора asPop и записать результаты разметки в ряд markup
  int enter (std::vector<int>& markup, const std::vector<double>& row, const unsigned long begin, const unsigned long end);
  
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_POP_ELEM_EXT_HXX */
