/***************************************************************************
*					Preprocess
*
*	Description:	Абстрактный класс описания алгоритмов предобработки
*					исходных данных.
*	Author:			dk
*	History:
*
****************************************************************************/

#ifndef _Preprocess_HXX
#define _Preprocess_HXX

#include "AxiomLibException.h"
#include "Round.h"
#include "DataSet.h"
#include "Environment.h"

#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

class TemplateRecognizer;

class Preprocess {

 private:

 protected:

 public:

	// Деструктор
	virtual ~Preprocess () {}

	// Функция устанавливает параметры класса по данным, полученным из класса окружения
	virtual int initFromEnv (const Environment& env) = 0;

	// Функция создания копии алгоритма предобработки
	virtual Preprocess* copy (void) = 0;

	// Функция обучения данного алгоритма предобработки
	virtual double learn (TemplateRecognizer &templateRecognizer) = 0;

	// Основная функция класса - запуск алгоритма предобработки
	virtual int run (DataSet &dataSet) = 0;

}; // end of class

}; // end of namespace

#endif /* _Preprocess_HXX */
