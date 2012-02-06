/****************************************************************************
*				CompareStatisticSimple
*
*	Description:	Класс подсчета числа ошибок первого и воторого рода
*	Author:		dk,gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_COMPARE_STATISTIC_SIMPLE_HXX
#define __AXIOMS_LIB_COMPARE_STATISTIC_SIMPLE_HXX

#include "./elem_conditions/ElemCondition.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "CompareStatistic.h"
#include "all_axioms.h"
#include "Axiom.h"
#include <math.h>
#include <iostream>      
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

class CompareStatisticSimple : public CompareStatistic {
	private:
	
	// Параметры класса задающие область, в которой искать соответсвие для разметки 
	// то есть - если распознали какой либо образ - то соответсвующий ему ищем в корректной разметке в заданной окрестности
	// число точек учитываемых слева
	int left;
	
	// число точек учитываемых справа
	int right;

	int ImH; // Very important param - максимальное превышение числа найденных траеторий над реально пресутсвующими 
		     // 				  - после чего начиаем учитывать это превышение в first

	protected:

	public:

	// Стандартный конструктор с заданием параметров по умолчанию
	CompareStatisticSimple (void);

	// Возвращает имя данного класса кодсчета статистики размекти
	std::string name (void);
	
	// Функции задания значений параметров класса
	int setParams (const int newLeft, const int newRight, const int newImH);
	int setParams (std::map<std::string, std::string> &params);
	int setParamsFromEnv (const Environment& env);

	// Функция возвращает копию данного класса
	CompareStatistic* copy (void);
	
	// Получить число ошибок первого и второго рода по ряду и корректной разметке
	int getStatistic (const std::vector<int> &row, const std::vector<int> &correct, double &first, double &second, const bool comments);

};

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_COMPARE_STATISTIC_SIMPLE_HXX */
