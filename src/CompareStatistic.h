/****************************************************************************
*				CompareStatistic
*
*	Description:	Общий предок классов подсчитывания статистки разметки
*	Author:		dk,gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_COMPARE_STATISTIC_HXX
#define __AXIOMS_LIB_COMPARE_STATISTIC_HXX

#include "AxiomLibException.h"
#include "Environment.h"
#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

class CompareStatistic {
	private:

	protected:

	public:

	// Стандартный конструктор
	CompareStatistic (void) {}
	
	// Стандартный деструктор
	virtual ~CompareStatistic() {}

	// Возвращает имя данного класса кодсчета статистики размекти
	virtual std::string name (void) = 0;

	// Возвращает параметры данного класса подсчета статистики
	//virtual int getParams (std::map<std::string, std::string> &params) = 0;
	
	// Функция задания значений параметров класса
	virtual int setParams (std::map<std::string, std::string> &params) = 0;

	// Функция задания значений параметров класса из окружения
	virtual int setParamsFromEnv (const Environment& env) = 0;

	// Возвращает копию класса подсчета статистики
	virtual CompareStatistic* copy (void) = 0;
	
	// Получить ошибки - первого и второго рода
	virtual int getStatistic (const std::vector<int> &row, const std::vector<int> &correct, double &first, double &second, const bool comments) = 0;

};

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_COMPARE_STATISTIC_HXX */
