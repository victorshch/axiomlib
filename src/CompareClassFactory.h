/****************************************************************************
*				CompareClassFactory
*
*	Description:	
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_COMPARE_CLASS_FACTORY_HXX
#define __AXIOMS_LIB_COMPARE_CLASS_FACTORY_HXX

#include <iostream>
#include <string>
#include <vector>

#include "CompareStatistic.h"

namespace AxiomLib {

class CompareClassFactory {

 private:

 protected:
 
 public:

  // Конструктор, инициализирует фабрику для порождения классов
  CompareClassFactory(void);
  
  // Декструктор
  ~CompareClassFactory(void);
  
  // Создает класс подсчета целевой функции 
  CompareStatistic* create (const std::string &name);
  
};

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_COMPARE_CLASS_FACTORY_HXX */
