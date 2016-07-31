/****************************************************************************
*
*				Функции класса TemplateGeneticsFactory
*
****************************************************************************/

#include "TemplateGeneticsFactory.h"

using namespace AxiomLib;

/****************************************************************************
*					TemplateGeneticsFactory::create
*
*	Description:	Создает класс алгоритма обучения шаблонов
*	Parameters:		name - имя класса, экземпляр которого создаем
*	Returns:		TemplateGenetics* - указатель на созданный класс
*	Throws:			AxiomLibException - если класса нет
*	Author:			dk
*	History:
*
****************************************************************************/
TemplateGenetics* TemplateGeneticsFactory::create (const std::string name) {
	
	if ( (name == "Extend") || (name == "EXTEND") || (name == "extend") ) {
		TemplateGeneticsExt* toReturn;
		toReturn = new TemplateGeneticsExt;
		return toReturn;
	}

	throw AxiomLibException ("TemplateGeneticsFactory::create : can not find the requested TemplateGenetics class.");
}
