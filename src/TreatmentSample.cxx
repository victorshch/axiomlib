/****************************************************************************
*				TreatmentSample
*
*	Description:	Функции класса TreatmentSample - Реализация
*	Author:		dk
*	History:		
*
****************************************************************************/
#include "TreatmentSample.h"
#include "AxiomLibException.h"

using namespace AxiomLib;

/****************************************************************************
*			TreatmentSample::setParamsFromEnv
*
*	Description:	Установка параметров класса
*	Parameters:	const Environment& envStart - откуда ставить
*	Returns:		0
*	Throws:		AxiomLibException - если параметра нет
*	Author:		dk
*	History:
*
****************************************************************************/
int TreatmentSample::setParamsFromEnv (const Environment& env) {
	
	return 0;
}


/****************************************************************************
*				TreatmentSample::addRes
*
*	Description:	Функция добавления к истории текущего класса значения целевой функции после подбора параметров
*	Parameters:	res - добавляемое значение целевой функцции полученной на некоторой системе аксиом при данном классе предобработки рядов
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
int TreatmentSample::addRes (double res) {
	
	return 0;
}


/****************************************************************************
*			TreatmentSample::change
*
*	Description:	Функция изменения параметров класса предобработки
*	Parameters:	-
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
int TreatmentSample::change (void) {
	
	return 0;
}


/****************************************************************************
*			TreatmentSample::process
*
*	Description:	Провести операцию предобработки над входным набором данных
*	Parameters:	dataSet - входной набор данных дял преобразования
*				newDataSet - преобразованный набор данных
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
int TreatmentSample::process (DataSet &newDataSet, const DataSet &dataSet) const {
	newDataSet = dataSet;
	return 0;
}
