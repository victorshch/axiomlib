/****************************************************************************
*			TreatmentSample
*
*	Description:	Пример класса предобработки рядов
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_TREATMENT_SAMPLE_HXX
#define __AXIOMS_LIB_TREATMENT_SAMPLE_HXX

#include "Treatment.h"
#include "AxiomSetPop.h"
#include <string>
#include <stdlib.h>

namespace AxiomLib {

class TreatmentSample : public Treatment{

private:

protected:

public:
  
	// Стандартный конструктор
	TreatmentSample (void) {}

	// Стандартный деструктор
	~TreatmentSample () {}

	// Возвращает имя данного класса предобработки
	std::string name (void) const {return (std::string) "TreatmentSample";}

	// Функция задания значений параметров класса
	int setParamsFromEnv (const Environment& envStart);

	// Функция добавления к истории текущего класса значения целевой функции после подбора параметров
	int addRes (double res);

	// Функция изменения параметров класса предобработки
	// НЕОБХОДИМО ИЗМЕНИТЬ ЕЕ ИНТЕРФЕЙС, ЧТОБЫ УЧИТЫВАТЬ ИСТОРИЮ ДАННОГО КЛАССА И НОМЕР ИТЕРАЦИИ ЦИКЛА ПО ВЫБОРУ ОПТИМАЛЬНОГО КЛАССА ПРЕДОБРАБОТКИ
	int change (void);

	// Провести операцию предобработки над входным набором данных
	int process (DataSet &newDataSet, const DataSet &dataSet) const;

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_TREATMENT_SAMPLE_HXX */
