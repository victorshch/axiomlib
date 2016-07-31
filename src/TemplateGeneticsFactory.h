/****************************************************************************
*				TemplateGeneticsFactory
*
*	Description:	Фабричный метод создания классов распознавания
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef _AXIOMS_LIB_TEMPLATE_GENETICS_FACTORY_HXX
#define _AXIOMS_LIB_TEMPLATE_GENETICS_FACTORY_HXX

#include "AllTemplateGenetics.h"
#include "AxiomLibException.h"
#include <string>

namespace AxiomLib {

class TemplateGeneticsFactory {

 private:

 protected:
 
 public:

	// Конструктор класса фабрики
	TemplateGeneticsFactory(void) {}

	// Деструктор класса фабрики
	~TemplateGeneticsFactory(void) {}
  
	// Функция создания объекта класса TemplateGenetics по названию класса
	TemplateGenetics* create (const std::string name);
  
}; // end of class

}; // end of namespace AxiomLib

#endif /* _AXIOMS_LIB_TEMPLATE_GENETICS_FACTORY_HXX */
