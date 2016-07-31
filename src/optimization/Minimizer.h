/****************************************************************************
*					Minimizer
*
*	Description:	Интерфейс алгоритма минимизации функции одного переменного.
*	Author:			wictor
*	History:	
*
****************************************************************************/
#ifndef MINIMIZER_H
#define MINIMIZER_H

#include "FunctionOneVariable.h"
#include "../Environment.h"

namespace AxiomLib {
namespace Optimization {

class Minimizer
{
private:
    Minimizer(const Minimizer& other);
    Minimizer& operator=(const Minimizer& other);
public:
	Minimizer();
	
	virtual ~Minimizer() {}
	
    // Инициализация параметров минимизатора из окружения
	virtual void initFromEnv(const Environment& env) = 0;
	
    // Поиск минимума заданной функции в заданных пределах
	virtual void findMinimum(const FunctionOneVariable* function, double lowerBound, double upperBound,
							 double& bestX, double& bestF) = 0;
};

}

}

#endif // MINIMIZER_H
