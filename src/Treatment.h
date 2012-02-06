/****************************************************************************
*				Treatment
*
*	Description:	Родительский класс для операций предобработки
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_TREATMENT_HXX
#define __AXIOMS_LIB_TREATMENT_HXX

#include "Environment.h"
#include "AxiomLibException.h"
#include "DataSet.h"
#include <string>
#include <vector>

namespace AxiomLib {

class Treatment {

private:

protected:

public:

	// Стандартный конструктор - пустой
	Treatment (void) {}

	// Стандартный деструктор - пустой
	virtual ~Treatment () {}

	// Возвращает имя данного класса предобработки
	virtual std::string name (void) const = 0;

	// Функция задания значений параметров класса
	virtual int setParamsFromEnv (const Environment& env) = 0;

	// Функция добавления к истории текущего класса значения целевой функции после подбора параметров
	virtual int addRes (double res) = 0;

	// Функция изменения параметров класса предобработки
	// НЕОБХОДИМО ИЗМЕНИТЬ ЕЕ ИНТЕРФЕЙС, ЧТОБЫ УЧИТЫВАТЬ ИСТОРИЮ ДАННОГО КЛАССА И НОМЕР ИТЕРАЦИИ ЦИКЛА ПО ВЫБОРУ ОПТИМАЛЬНОГО КЛАССА ПРЕДОБРАБОТКИ
	virtual int change (void) = 0;

	// Провести операцию предобработки над входным набором данных
	virtual int process(DataSet &newDataSet, const DataSet &dataSet) const = 0;

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_TREATMENT_HXX */
