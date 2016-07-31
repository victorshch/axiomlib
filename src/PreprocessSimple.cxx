/****************************************************************************
*
*				Функции класса PreprocessSimple
*
****************************************************************************/

#include "PreprocessSimple.h"
#include "TemplateRecognizer.h"

using namespace AxiomLib;

// Коструктор класса
PreprocessSimple::PreprocessSimple (void) {
	someParameter = 0;
} 

// Деструктор класса
PreprocessSimple::~PreprocessSimple (void) {
	
}

/****************************************************************************
*						PreprocessSimple::initFromEnv
*
*       Description:	Функция устанавливает параметры класса по данным, 
*						полученным из класса окружения
*       Parameters:		env - окружение, содержашее необходимые параметры
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int PreprocessSimple::initFromEnv (const Environment& env) {
	
	return 0;
}


/****************************************************************************
*						PreprocessSimple::copy
*
*       Description:	Функция создания копии алгоритма предобработки
*       Parameters:		void
*       Returns:		Preprocess* - указатель на созданную копию данного предобработчика
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
Preprocess* PreprocessSimple::copy (void) {
	PreprocessSimple* toReturn;
	toReturn = new PreprocessSimple;
	toReturn->setParams (this->someParameter);
	return (Preprocess*) toReturn;
}


/****************************************************************************
*						PreprocessSimple::setParams
*
*       Description:	Функция устанавливает специфичные для данного класса параметры
*       Parameters:		sp - соответствует переменной класса someParameter
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int PreprocessSimple::setParams (int sp) {
	someParameter = sp;
	return 0;
}


/****************************************************************************
*						PreprocessSimple::learn
*
*       Description:	Функция обучения данного алгоритма предобработки
*       Parameters:		templateRecognizer - шаблон распознавателя, на котором
*						проверять качество обучения данного алгоритма предобработки
*       Returns:		-1
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
double PreprocessSimple::learn (TemplateRecognizer &templateRecognizer) {

	return -1;
}


/****************************************************************************
*						PreprocessSimple::run
*
*       Description:	Основная функция класса - запуск алгоритма предобработки
*       Parameters:		dataSet - преобразуемый набор данных
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int PreprocessSimple::run (DataSet &dataSet) {

	return 0;
}
