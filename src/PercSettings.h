/****************************************************************************
*						PercSettings.h
*
*       Description:	Объявление структуры PercSettings
*       Author:			wictor
*       History:
*
****************************************************************************/
#ifndef PERCSETTINGS_H_
#define PERCSETTINGS_H_

#include <vector>

#include "NeuralFANNHeader.h"

#include "Environment.h"
#include <string>

namespace AxiomLib {

/****************************************************************************
*						PercSettings
*
*       Description:	Структура, содержащая настройки многослойного перцептрона
*						и функцию инициализации из окружения
*       Author:			wictor
*       History:
*
****************************************************************************/
struct PercSettings {
	// Число рабочих слоев (считаются все слои кроме входного)
	int numLay;
	// Факторы числа нейронов в слое
	std::vector<double> numNeuronsF;
	// Активационные функции слоев
	std::vector<FANN::activation_function_enum> actFuncs;
	// Параметры (s) активационных функций по слоям
	std::vector<double> actFuncsS;
	// Алгоритм обучения
	FANN::training_algorithm_enum trainingAlgorithm;
	// Коэффициент обучения
	double learningRate;
	// Момент (инерция) обучения
	double learningMomentum;
	// Максимальное число эпох обучения
	int maxIters;
	// Число итераций между репортами об обучении
	int itersBetweenReports;
	// Желаемая ошибка (на которой заканчивается обучение)
	double desiredError;
	//FANN::callback_type callback;
	
	// Проверка набора настроек на корректность
	// TODO: дописать
	bool valid() const { 
		return (numLay - 1) == numNeuronsF.size() &&
			numLay == actFuncs.size() &&
			numLay == actFuncsS.size() &&
			numLay > 0 &&
			learningRate > 0.0 &&
			maxIters > 0;
	}
	
	// Инициализация из окружения
	void initFromEnv(const Environment &env);
	
};

};

#endif