/****************************************************************************
*				StartStrategy
*
*	Description:	Класс стратегии выбора предобработчика с параметрами
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_START_STRATEGY_H
#define __AXIOMS_LIB_START_STRATEGY_H

#include "Environment.h"
#include "EnvDataSet.h"
#include "AxiomSetPop.h"
#include "AxiomLibException.h"
#include "BaseGenetics.h"
#include "Treatment.h"
#include "TreatmentFactory.h"
#include <string>
#include <vector>
#include <iostream>

namespace AxiomLib {

class StartStrategy {

private:

	// вектор используемых классов генетического подбора систем аксиом
	std::vector <BaseGenetics> gaVector;

	// вектор используемых популяций генетического алгоритма
	std::vector <AxiomSetPop> popVector;

	// Исходный набор данных - считываем его однажды и больше не меняем
	DataSet dataSet;

	// вектор индексов параметров в каноническом представлении по которым вести анализ - поиск нештатных ситуаций
  	// под параметром понимается - название одномерного ряда в множестве рядов описывающих поведение системы
	std::vector<int> params;

	// Данный класс не должен иметь инфы о предобработчиках - только ссылки на них и все 
	// А уже предобработчики - должны хранить свою историю и иметь функции измененения собствненных параметров 
	// с учетом истории, те есть параметров и результатов (топовых значений целевой функции)
	std::vector <Treatment * > treatVector;

	// Строка с именем конфига для классов генетического алгоритма
	std::string bgConfigFile;

	// Число стартов с каждым из классов предобработки с разлчиными параметрами (считывается из конфигурационного файла)
	int numOfStarts;

protected:

	// вспомогательная функция сортировки индексов в векторе mask по значениям в векторе resVector
	inline int sort (std::vector<int> &mask, std::vector<double> &resVector) const;

	// вспомогательная функция сокращения числа рассматриваемых преобработчиков по велечине показанной целевой функции
	inline int reduce(const int i, const std::vector<int> &mask);
	
	// вспомогательная функция удаления заданных предобработчиков из вектора таковых
	inline int deleteProcs (std::vector<bool> &alive);

	// вспомогательная функция подсчета числа операций ГА на данном этапе выбора класса предобработки
	inline int countNumOfIters (const int i);

	// вспомогательная функция сохранения в файл всех данных по i-ому предобработчику
	int saveToFile (const int i) const;

public:

	// Пустой конструктор с заданием параметров по умолчанию
	StartStrategy (void);

	// Деструктор - удаляются все созданные в рамках данного класса динамические объекты
	~StartStrategy(void);

	// Функция инициализации данного класса - задания начальных парметров внутренних переменных
	void init (void);

	// основная функция запуска стратегии выбора предобработчика
	void start (void);

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_START_STRATEGY_H */
