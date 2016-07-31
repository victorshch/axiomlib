/****************************************************************************
*			TransmutePrecision
*
*	Description:	Функции класса TransmutePrecision - Реализация
*	Author:		gevor
*	History:		
*
****************************************************************************/
#include "TransmutePrecision.h"
#include "AxiomLibException.h"

using namespace AxiomLib;

/****************************************************************************
*			TransmutePrecision::setParamsFromEnv
*
*	Description:	Установка параметров мутации из среды
*	Parameters:	const Environment& env - откуда ставить
*	Returns:		0
*	Throws:		AxiomLibException - если параметра нет
*	Author:		gevor
*	History:
*
****************************************************************************/
int TransmutePrecision::setParamsFromEnv (const Environment& env)
{
  if (env.getDoubleParamValue(ecTransLevel, "transLevel") < 0) {
    throw AxiomLibException ("Error in TransmutePrecision::setParamsFromEnv - can not find all needed parameners.");
  }
  if (ecTransLevel < 0 || ecTransLevel > 1)
    throw AxiomLibException ("Error in TransmutePrecision::setParamsFromEnv - ecTransLevel out of range.");
  return 0;
}

/****************************************************************************
*			TransmutePrecision::select
*
*	Description:	Функция мутации для данной популяции - мутация только на уровне элементарных условий (пока)
*	Parameters:	AxiomSetPop - изменяемая популяция
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
int TransmutePrecision::transmute (AxiomSetPop& pop) {
	// проверка соответстивя параметров границам
	if (ecTransLevel < 0 || ecTransLevel > 1)
		throw AxiomLibException ("Error in TransmutePrecision::transmute.");
	// ходим по популяции, мутируем ее, попутно сбрасывая статистику
	// мутация на уровне параметров элементарных условий
	if (ecTransLevel > 0) {
		// получаем вектор
		initTransmute (pop);
		int curPopSize = (int) axiomSets.size();
		// ходим по всем наборам аксиом
		for (int i = 0; i < curPopSize; i++) {
			int nOfAxioms = (int) (axiomSets[i])->size();
			int resTransmuteAxiom = 0;
			// ходим по всем аксиомам в системе
			for (int j = 0; j < nOfAxioms; j++) {
				resTransmuteAxiom = (*(axiomSets[i]))[j]->transmute(ecTransLevel, 0.0);
			}
			(*asPop)[i].voidStats();
		}
	}
	
	return 0;
}
