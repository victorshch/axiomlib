/****************************************************************************
*			SimpleCrossoverStrategy
*
*	Description:	Функции класса SimpleCrossoverStrategy - Реализация
*	Author:		gevor
*	History:		
*
****************************************************************************/
#include "SimpleCrossoverStrategy.h"
#include "AxiomLibException.h"
#include "GaOps.h"

using namespace AxiomLib;
using namespace std;

/****************************************************************************
*			SimpleCrossoverStrategy::setParamsFromEnv
*
*	Description:	Установка параметров мутации из среды
*	Parameters:	const Environment& env - откуда ставить
*	Returns:	0
*	Throws:		AxiomLibException - если параметра нет
*	Author:		gevor
*	History:
*
****************************************************************************/
int SimpleCrossoverStrategy::setParamsFromEnv (const Environment& env)
{
  if (env.getDoubleParamValue(pCross, "crossLevel") < 0) {
    throw AxiomLibException ("Error in CrossoverSimple::setParamsFromEnv - can not find all needed parameners.");
  }
  if (pCross < 0 || pCross > 1)
    throw AxiomLibException ("Error in CrossoverSimple::setParamsFromEnv - pCross out of range.");
  return 0;
}


/****************************************************************************
*				SimpleCrossoverStrategy::setParams
*
*	Description:	Функция задания значений параметров 
*	Parameters:	новые параметры скрещивания:
*				newElemCondCrossLevel - велечина скрещивания на уровне элементарных условий 
*				newAxiomCrossLevel - велечина скрещивания на уровне аксиом
*				newAxiomSetCrossLevel - велечина скрещивания на уровне систем аксиом
*	Returns:		0
*	Throws:		AxiomLibException - если входные значения параметров некорректное
*	Author:		dk
*	History:
*
****************************************************************************/
int SimpleCrossoverStrategy::setParams (const double newElemCondCrossLevel, const double newAxiomCrossLevel, const double newAxiomSetCrossLevel) {
	// Проверка на соответствие граничным значениям параметров
	if (	(newElemCondCrossLevel < 0.0) || (newElemCondCrossLevel > 1.0) ||
				(newAxiomCrossLevel < 0.0) || (newAxiomCrossLevel > 1.0) ||
				(newAxiomSetCrossLevel < 0.0) || (newAxiomSetCrossLevel > 1.0) 	) 
		throw AxiomLibException ("Error in SimpleCrossoverStrategy::setParams: some of crossLevel Parameters are out of range.");
	// Установка новых значений параметров
	pCross = (newElemCondCrossLevel + newAxiomCrossLevel + newAxiomSetCrossLevel) / 3.0;
	return 0;
}


/****************************************************************************
*				SimpleCrossoverStrategy::setParams
*
*	Description:	Функция задания значений параметров 
*	Parameters:	новые параметры скрещивания:
*				newCrossLevel - велечина скрещивания на всех уровнях
*	Returns:		0
*	Throws:		AxiomLibException - если входные значения параметров некорректное
*	Author:		dk
*	History:
*
****************************************************************************/
int SimpleCrossoverStrategy::setParams (const double newCrossLevel) {
	// Проверка на соответствие граничным значениям параметров
	if ( (newCrossLevel < 0.0) || (newCrossLevel > 1.0) ) 
		throw AxiomLibException ("Error in SimpleCrossoverStrategy::setParams: given crossLevel is out of range.");
	// Установка новых значений параметров
	pCross = newCrossLevel;
	return 0;
}


/****************************************************************************
*			SimpleCrossoverStrategy::crossover
*
*	Description:	Функция скрещивания для данной популяции - пока только на уровне
*			параметров элементарных условий
*	Parameters:	AxiomSetPop - изменяемая популяция
*	Returns:	0
*	Throws:		-
*	Author:		gevor
*	History:
*
****************************************************************************/
int SimpleCrossoverStrategy::crossover (AxiomSetPop& pop) 
{
  // Простой, полностью случайный метод - скрещиваем pCross процентов популяции
  if (pCross < 0)
    throw AxiomLibException ("Error in CrossoverSimple::crossover - pCross unititialized.");

  // получаем вектор систем аксиом
  initCrossover (pop);
  
  int popSize = asPop->size();
  int nCross = (int)(pCross * ((double)popSize)); // сколько участвуют в скрещивании
  int newSize = nCross;// + popSize; // новый размер популяции
  
  // создаем новый вектор
  vector<AxiomSetWithStats>* newAsPop = new vector<AxiomSetWithStats>;
  newAsPop->resize(newSize);
  
  // Пока есть нескрещенные аксиомы - случайным образом выбираем пары и скрещиваем, записывая в новый вектор
  for (int i = 0; i < newSize; i++) {
    // выбираем случайным образом 2 системы аксиом
    int firstASNum = (int) (( (double) rand() / (double) RAND_MAX) * ((double)popSize  - 1.0));
    int secondASNum = (int) (( (double) rand() / (double) RAND_MAX) * ((double)popSize  - 1.0));

    // проверка алгоритма Димы, убрать, если все будет работать нормально
    if (firstASNum >= popSize || secondASNum >= popSize || firstASNum < 0 || secondASNum < 0)
      throw AxiomLibException ("Error in CrossoverSimple::crossover - FATAL INTERNAL ERROR 1.");
    
    // теперь ходим по этим системам аксиом, попарно скрещивая аксиомы, а в них - элементарные
    // условия; если структурная идентичность не соблюдается - что-то внизу ругнется
    CrossoverOp_Params((*newAsPop)[i], (*asPop)[firstASNum], (*asPop)[secondASNum]);

//    (*newAsPop)[i] = (*asPop)[0];

    // обнуляем статистику для результата скрещивания
    (*newAsPop)[i].voidStats();
  }
  
  // добавляем новые элементы в популяцию систем аксиом
  pop.addSets(*newAsPop);
  return nCross;
}
