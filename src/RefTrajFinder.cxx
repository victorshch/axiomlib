/****************************************************************************
*
*				Функции класса RefTrajFinder
*
****************************************************************************/

#include "RefTrajFinder.h"

using namespace AxiomLib;

// Коструктор класса - задает значения элементам класса по умолчанию
RefTrajFinder::RefTrajFinder (void) {
	
} 


// Деструктор класса - удаляюся все созданные в рамках класса динамические объекты
RefTrajFinder::~RefTrajFinder (void) {
	
}


/****************************************************************************
*						RefTrajFinder::setDataSet
*
*       Description:	Функция установки нового набора данных в данный класс
*       Parameters:		newDataSet - новый набор данных
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int RefTrajFinder::setDataSet (const DataSet &newDataSet) {
	dataSet = newDataSet;
	return 0;
}


/****************************************************************************
*                       RefTrajFinder::setParamsOfDataSet
*
*       Description:	Функция установки нового значаения переменой класса params
*       Parameters:		newParams - новое значение вектора
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int RefTrajFinder::setParamsOfDataSet (std::vector<int> &newParams) {
	params = newParams;
	return 0;
}


/****************************************************************************
*                       RefTrajFinder::copy
*
*       Description:	Функция возвращает копию данного класса
*       Parameters:		-
*       Returns:		RefTrajFinder* - ссылка на создаваемую копию данного класса
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
RefTrajFinder* RefTrajFinder::copy (void) {
	RefTrajFinder* toReturn;
	toReturn = new RefTrajFinder;
	toReturn->setDataSet (dataSet);
	toReturn->setParamsOfDataSet (params);
	return (RefTrajFinder*) toReturn;
}


/****************************************************************************
*				RefTrajFinder::initFromEnv
*
*	Description:	Функция устанавливает параметры объекта по данным из 
*					окружения, которое подается как параметр функции
*	Parameters:	env - объект класса окружения, откуда берутся параметры данного класса
*	Returns:	0
*	Throws:		AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:		dk
*	History:
*
****************************************************************************/
int RefTrajFinder::initFromEnv (const Environment& env) {
	
	return 0;
}


/****************************************************************************
*					RefTrajFinder::run
*
*	Description:	Основная функция в классе - реализует алгоритм работы
*					поиска эталонных траекторий.
*	Parameters:	void
*	Returns:	0
*	Throws:		AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:		dk
*	History:
*
****************************************************************************/
int RefTrajFinder::run (void) {

	return 0;
}
