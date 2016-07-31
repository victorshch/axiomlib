/****************************************************************************
*					HybridGridSearch
*
*	Description:	Поиск минимума разбитием области поиска равномерной сеткой
*                   и применением заданного алгоритма в каждом отрезке разбиения
*	Author:			wictor
*	History:
*
****************************************************************************/
#ifndef HYBRIDGRIDSEARCH_H
#define HYBRIDGRIDSEARCH_H

#include <memory>

#include "Minimizer.h"

namespace AxiomLib {
namespace Optimization {

class HybridGridSearch : public Minimizer
{
private:
	std::auto_ptr<Minimizer> minimizer;
	int numPartitions;
	
public:
	HybridGridSearch();

    // Инициализация параметров минимизатора из окружения
	void initFromEnv(const Environment &env);
	
    // Поиск минимума заданной функции в заданных пределах
	void findMinimum(const FunctionOneVariable *function, double lowerBound, double upperBound, double &bestX, double &bestF);
};

}
}

#endif // HYBRIDGRIDSEARCH_H
