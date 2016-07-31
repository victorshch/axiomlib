/****************************************************************************
*			SimpleTransmuteStrategy
*
*	Description:	Функции класса SimpleTransmuteStrategy - Реализация
*	Author:		gevor
*	History:		
*
****************************************************************************/
#include "SimpleTransmuteStrategy.h"
#include "AxiomLibException.h"

using namespace AxiomLib;

/****************************************************************************
*			SimpleTransmuteStrategy::setParamsFromEnv
*
*	Description:	Установка параметров мутации из среды
*	Parameters:	const Environment& env - откуда ставить
*	Returns:		0
*	Throws:		AxiomLibException - если параметра нет
*	Author:		dk
*	History:
*
****************************************************************************/
int SimpleTransmuteStrategy::setParamsFromEnv (const Environment& env) {
	double tempDB1, tempDB2, tempDB3, tempDB4;
	if ( (env.getDoubleParamValue(tempDB1, "elCondTransLevel") >= 0) && (env.getDoubleParamValue(tempDB2, "axiomTransLevel") >= 0) && (env.getDoubleParamValue(tempDB3, "axSysTransLevel") >= 0) && (env.getDoubleParamValue(tempDB4, "priorityTransLevel") >= 0) ) {
		if ( (tempDB1 < 0) || (tempDB1 > 1) || (tempDB2 < 0) || (tempDB2 > 1) || (tempDB3 < 0) || (tempDB3 > 1) || (tempDB4 < 0) || (tempDB4 > 1) )
			throw AxiomLibException ("Error in SimpleTransmuteStrategy::setParamsFromEnv - transmute levels out of range.");
		ectp = tempDB1;
		atp = tempDB2;
		stp = tempDB3;
		sptp = tempDB4;
		//std::cout << "\n	paramsSet: " << ectp << " " << atp << " " << stp << " " << sptp;
		return 1;
	}
	if (env.getDoubleParamValue(tempDB1, "transLevel") >= 0) {
		if ((tempDB1 < 0) || (tempDB1 > 1))
			throw AxiomLibException ("Error in SimpleTransmuteStrategy::setParamsFromEnv - transLevel out of range.");
		ectp = tempDB1;
		atp = tempDB1;
		stp = tempDB1;
		sptp = tempDB1;
		return 2;
	}
	throw AxiomLibException ("Error in SimpleTransmuteStrategy::setParamsFromEnv - can not find all needed parameners.");
	return -1;
}


/****************************************************************************
*				SimpleTransmuteStrategy::transmuteAxiomSet
*
*	Description:	Функция мутации системы аксиом
*	Parameters:	asNum - номер системы аксиом в векторе axiomSets для мутации
*	Returns:		0 - если все произошло успешно
*				-1 - если мутация не прошла
*	Throws:		AxiomLibExeption - если входные параметры не соответствуют области их задания [0, 1]
*	Author:		dk
*	History:
*
****************************************************************************/
int SimpleTransmuteStrategy::transmuteAxiomSet (unsigned int asNum) {
	//проверка значений параметров мутации на соответсвие области значений
	if ((ectp < 0) || (ectp > 1) || (atp < 0) || (atp > 1) || (stp < 0) || (stp > 1) || (sptp < 0) || (sptp > 1)) {
		throw AxiomLibException("Error in SimpleTransmuteStrategy::transmuteAxiomSet : one of parameteres is out of range.");
		return -1;
	}

	//мутация на уровне аксиом и элементарынх условий
	if ((ectp > 0) || (atp > 0)) {
		int resTransmuteAxiom = 0;
		for (unsigned int i = 0; i < (axiomSets[asNum])->size(); i++) {
			resTransmuteAxiom = ((*(axiomSets[asNum]))[i])->transmute (ectp, atp);
		}
	}

	//мутация на уровне системы аксиом
	if (stp > 0) {
		double probOfTp = 0;
		int numOfEC = 0;
		for (unsigned int i = 0; i < (axiomSets[asNum])->size(); i++) {
			probOfTp = (double) rand() / (double) RAND_MAX;
			if (probOfTp < stp) {
				numOfEC = ((*(axiomSets[asNum]))[i])->getNumOfEC();
				((*(axiomSets[asNum]))[i])->clear();
				((*(axiomSets[asNum]))[i])->initAxiomByRand(numOfEC);
			}
		}
	}


	//мутация на уровне изменения приоритета аксиом в системе
	if (sptp > 0) {
		int numOfSteps = 0;
		double probChange = 0.0;
		for (int i = 1; i < (int) (axiomSets[asNum])->size(); i++) {
			probChange = (double) rand() / (double) RAND_MAX;
			if (probChange < sptp) {
				numOfSteps = round( ((double) rand() / (double) RAND_MAX) * i * sptp );
				if (numOfSteps > 0) {
					((*asPop)[asNum]).slide(i-numOfSteps, i);
				}
			}
		}
	}

	return 0;
}


/****************************************************************************
*			SimpleTransmuteStrategy::transmute
*
*	Description:	Функция мутации для данной популяции - мутация только на уровне элементарных условий (пока)
*	Parameters:	AxiomSetPop - изменяемая популяция
*	Returns:		0
*	Throws:		-
*	Author:		gevor
*	History:
*
****************************************************************************/
int SimpleTransmuteStrategy::transmute (AxiomSetPop& pop) {
	// проверка соответстивя параметров границам
	// ходим по популяции, мутируем ее, попутно сбрасывая статистику
	initTransmute (pop);
	for (unsigned int i = 0; i < axiomSets.size(); i++) {
		transmuteAxiomSet (i);
		(*asPop)[i].voidStats();
	}
	return 0;
}
