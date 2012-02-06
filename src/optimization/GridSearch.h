/****************************************************************************
*					GridSearch
*
*	Description:	Поиск минимума простым вычислением значений на равномерной сетке.
*	Author:			wictor
*	History:	
*
****************************************************************************/
#ifndef GRIDSEARCH_H
#define GRIDSEARCH_H

#include "../Environment.h"
#include "Minimizer.h"

namespace AxiomLib {
namespace Optimization {

class GridSearch : public Minimizer
{
	int numPartitions;
public:
	GridSearch();
	
    // Инициализация параметров минимизатора из окружения
	void initFromEnv(const Environment &env);
	
    // Поиск минимума заданной функции в заданных пределах
	void findMinimum(const FunctionOneVariable *function, 
					 double lowerBound, double upperBound, 
				     double &bestX, double &bestF);
};

}

}

#endif // GRIDSEARCH_H
