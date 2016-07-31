/****************************************************************************
*				NeuralCommonSettings.h
*
*	Description:	Файл, содержащий определение структур NeuralCommonSettings
*					и NeuralCoderSettings
*	Author:		wictor
*	History:	
*
****************************************************************************/

#ifndef NEURALCOMMONSETTINGS_H_
#define NEURALCOMMONSETTINGS_H_

#include "Environment.h"
#include <string>

namespace AxiomLib {

/****************************************************************************
*				NeuralCommonSettings
*
*	Description:	Структура, содержащия общие настройки нейросетевого алгоритма:
*					srcQ, minDist, comments, а также функцию инициализации из окружения
*	Author:		wictor
*	History:	
*
****************************************************************************/
struct NeuralCommonSettings {
	// Число символов алфавита
	int srcQ;
	// Минимальное расстояние до нормального поведения
	double minDist;
	// Флаг того, нужно ли выводить информацию в cout
	bool comments;
	// Флаг того, что для отсечения по расстоянию нужно использовать primary-строку
	// Иначе используется строка, номер которой возвращен классификатором
	bool useCentral;
	// Инициализация из окружения
	void initFromEnv(const Environment &env);
};

/****************************************************************************
*				NeuralCoderSettings
*
*	Description:	Структура, содержащия настройки кодировщика:
*					dstQ и numVar, а также функцию инициализации из окружения
*	Author:		wictor
*	History:	
*
****************************************************************************/
struct NeuralCoderSettings {
	// Число символов выходного алфавита кодировщика
	int dstQ;
	// Параметр num_var
	int numVar;
	// Параметр window_size
	int windowSize;
	// Инициализация из окружения
	void initFromEnv(const Environment &env);
};

};

#endif