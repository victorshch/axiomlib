/****************************************************************************
*
*			Функции класса CompareClassFactory
*
****************************************************************************/

#include <stdlib.h>
#include <math.h>
#include "AxiomLibException.h"
#include "Round.h"
#include "AllCompareStatistic.h"

#include "CompareClassFactory.h"

using namespace AxiomLib;

// Конструктор, инициализирует фабрику для порождения классов
CompareClassFactory::CompareClassFactory(void) {

}
  
// Декструктор
CompareClassFactory::~CompareClassFactory(void) {

}
  

/****************************************************************************
*			CompareClassFactory::create
*
*	Description:	Создает класс подсчета числа ошибок первого и второго рода
*	Parameters:	name - имя класса, экземпляр которого создаем
*	Returns:		CompareStatistic* - указательн на созданный класс
*	Throws:		AxiomLibException - если не описан в данной фабрике класс с данным в параметрах именем
*	Author:		dk
*	History:
*
****************************************************************************/
CompareStatistic* CompareClassFactory::create (const std::string &name) {
	if (name == "CompareStatisticSimple") {
		CompareStatisticSimple* toReturn;
		toReturn = new CompareStatisticSimple;
		return toReturn;
	}
	throw AxiomLibException ("Error in CompareClassFactory::create: given name of CompareClass is wrong");
}
