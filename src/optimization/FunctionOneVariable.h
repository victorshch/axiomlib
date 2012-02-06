/****************************************************************************
*					FunctionOneVariable
*
*	Description:	Интерфейс функции одного переменного.
*					Используется в алгоритмах поиска минимума.
*	Author:			wictor
*	History:	
*
****************************************************************************/
#ifndef FUNCTIONONEVARIABLE_H
#define FUNCTIONONEVARIABLE_H

namespace AxiomLib {

namespace Optimization {

class FunctionOneVariable
{
public:
	FunctionOneVariable();
	
	virtual double f(double x) const = 0;
};

}

}
#endif // FUNCTIONONEVARIABLE_H
