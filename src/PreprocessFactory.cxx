/****************************************************************************
*
*				Функции класса PreprocessFactory
*
****************************************************************************/

#include "PreprocessFactory.h"

using namespace AxiomLib;

// Контруктор фабрики классов
PreprocessFactory::PreprocessFactory(void) {
	classNum = 0;
	// ВНИМАНИЕ! Все классы должны быть занесены в данный вектор.
	classNames.resize (2);
	classNames[0] = "Simple";
	classNames[1] = "Squeeze";
}

// Деструктор фабрики классов
PreprocessFactory::~PreprocessFactory(void) {
	classNames.clear();
}

/****************************************************************************
*				PreprocessFactory::create
*
*	Description:	Создает класс предобработки по имени
*	Parameters:		name - имя класса, экземпляр которого создаем
*	Returns:		Preprocess* - указатель на созданный класс
*	Throws:			AxiomLibException - если класса нет
*	Author:			dk
*	History:
*
****************************************************************************/
Preprocess* PreprocessFactory::create (const std::string name) {
	
	if ( (name == "Simple") || (name == "SIMPLE") || (name == "simple") ) {
		PreprocessSimple* toReturn;
		toReturn = new PreprocessSimple;
		return toReturn;
	}

	if ( (name == "Squeeze") || (name == "SQUEEZE") || (name == "squeeze") ) {
		PreprocessSqueeze* toReturn;
		toReturn = new PreprocessSqueeze;
		return toReturn;
	}

	throw AxiomLibException ("PreprocessFactory::create : can not find the requested preprocessor.");
}


/****************************************************************************
*				PreprocessFactory::create
*
*	Description:	Создает очередной класс предобработки
*	Parameters:		-
*	Returns:		Preprocess* - указатель на созданный класс
*	Throws:			AxiomLibException - если класса нет
*	Author:			dk
*	History:
*
****************************************************************************/
Preprocess* PreprocessFactory::create (void) {
	// Проверка текущего значения переменной, отвечающий за очередность в создании классов
	if (classNum >= classNames.size())
		throw AxiomLibException ("PreprocessFactory::create : no preprocessors to create one.");
	// Создание класса предобработки
	Preprocess* toReturn;
	toReturn = create (classNames[classNum]);
	// Увеличение счетчика классов предобработки
	classNum++;
	if (classNum >= classNames.size())
		classNum = 0;
	// Возвращаем созданный класс предобработки
	return toReturn;
}


/****************************************************************************
*				PreprocessFactory::getSize
*
*	Description:	Возвращает число классов предобработки, которые могут быть 
*					созданы в данном классе.
*	Parameters:		-
*	Returns:		int - число классов предобработки
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int PreprocessFactory::getSize (void) {
	return (int) classNames.size();
}
