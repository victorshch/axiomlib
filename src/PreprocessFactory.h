/****************************************************************************
*					PreprocessFactory
*
*	Description:	Фабричный метод создания классов предобработки
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Preprocess_FACTORY_HXX
#define __AXIOMS_LIB_Preprocess_FACTORY_HXX

#include "AllPreprocess.h"
#include "AxiomLibException.h"

#include <string>
#include <vector>

namespace AxiomLib {

class PreprocessFactory {

  private:

	// Названия всех классов предобработки, которые могут быть созданы в данном классе
	std::vector <std::string> classNames;

	// Указатель, какой класс последний был создан с помощью функции create (void) или 0 вначале
	unsigned int classNum;

  protected:

  public:

	// Контруктор фабрики классов
	PreprocessFactory(void);

	// Деструктор фабрики классов
	~PreprocessFactory(void);

	// Возвращает число классов предобработки, которые могут быть созданы данным классом
	int getSize (void);

	// Функция создания объекта по имени
	Preprocess* create (const std::string name);

	// Функция создания объекта очередного класса предобработки
	Preprocess* create (void);

};

}; /* namespace AxiomLib */

#endif /* __AXIOMS_LIB_Preprocess_FACTORY_HXX */
