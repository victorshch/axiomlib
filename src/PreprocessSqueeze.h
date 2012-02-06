/***************************************************************************
*					PreprocessSqueeze
*
*	Description:	Класс предобработки, который в завимисимости от параметра
*					производит сжатие или растяжение траекторий из набора данных.
*	Author:			dk
*	History:
*
****************************************************************************/

#ifndef _Preprocess_Squeeze_HXX
#define _Preprocess_Squeeze_HXX

#include "AxiomLibException.h"
#include "Round.h"
#include "DataSet.h"
#include "Environment.h"
#include "Preprocess.h"
#include "GoalStrategyFactory.h"
#include "GoalStrategy.h"

#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

class PreprocessSqueeze : public Preprocess {

 private:

	// Параметр сжатия / растяжения траекторий (при 1.0 - траектория не изменяется)
	double squeezeParam;

	// Минимальное и максимальное значение для параметра squeezeParam
	double squeezeParamMin;
	double squeezeParamMax;

	// Степень полинома Лагранжа для многочлена интерполяции 
	// ( тоже что и (число точек интерполяции - 1) )
	unsigned int LagrangeDegree;

	// Вектор факториалов для вычисления значений множителей Лагранжа
	std::vector <double> factorials;

	GoalStrategy* goalStrategy; // Стратегия подсчета целевой функции по ошибкам I и II рода

	// Вспомогательная функция обучения - рекурсивно улучшает значение precision
	double sublearn (TemplateRecognizer &templateRecognizer, std::vector <double> &curRes, std::vector <double> &curSqueezeParams);

	// Вспомогательная функция - выбор преобразования для параметра изменения
	inline bool chooseBest (std::vector <double> &curRes, std::vector <double> &curSqueezeParams) const;

 protected:

 public:

	// Конструктор класса
	PreprocessSqueeze (void);

	// Деструктор класса
	~PreprocessSqueeze (void);

	// Функция устанавливает параметры класса по данным, полученным из класса окружения
	int initFromEnv (const Environment& env);

	// Функция устанавливает специфичные для данного класса параметры
	int setParams (double sp);

	// Функция устанавливает все специфичные для данного класса параметры
	int setParams (double sp, double spMin, double spMax, unsigned int ld);

	// Функция создания копии алгоритма предобработки
	Preprocess* copy (void);

	// Применение данного алгоритрма предобработки для одного отдельно взятого ряда
	bool apply (std::vector <double> &newVec, std::vector <double> &vec) const;

	// Применение данного алгоритрма предобработки для одного отдельно взятого ряда
	bool apply (std::vector <int> &newVec, std::vector <int> &vec) const;

	// Функция обучения данного алгоритма предобработки в рамках некоторого шаблона
	double learn (TemplateRecognizer &templateRecognizer);

	// Основная функция класса - запуск алгоритма предобработки
	int run (DataSet &dataSet);

}; // end of class

}; // end of namespace

#endif /* _Preprocess_Squeeze_HXX */
