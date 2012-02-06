/***************************************************************************
*					PreprocessSimple
*
*	Description:	Класс предобработки, который не изменяет данные.
*	Author:			dk
*	History:
*
****************************************************************************/

#ifndef _Preprocess_Simple_HXX
#define _Preprocess_Simple_HXX

#include "AxiomLibException.h"
#include "Round.h"
#include "DataSet.h"
#include "Environment.h"
#include "Preprocess.h"

#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

class PreprocessSimple : public Preprocess {

 private:

	// Некоторый параметр
	int someParameter;

 protected:

 public:

	// Конструктор класса
	PreprocessSimple (void);

	// Деструктор класса
	~PreprocessSimple (void);

	// Функция устанавливает параметры класса по данным, полученным из класса окружения
	int initFromEnv (const Environment& env);

	// Функция устанавливает специфичные для данного класса параметры
	int setParams (int sp);

	// Функция создания копии алгоритма предобработки
	Preprocess* copy (void);

	// Функция обучения данного алгоритма предобработки
	double learn (TemplateRecognizer &templateRecognizer);

	// Основная функция класса - запуск алгоритма предобработки
	int run (DataSet &dataSet);

}; // end of class

}; // end of namespace

#endif /* _Preprocess_Simple_HXX */
