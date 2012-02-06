/****************************************************************************
*					GoldenSectionSearch
*
*	Description:	Поиск минимума методом золотого сечения.
*					Реализация взята отсюда:
*					http://en.wikipedia.org/wiki/Golden_section_search
*	Author:			wictor
*	History:	
*
****************************************************************************/
#ifndef GOLDENSECTIONSEARCH_H
#define GOLDENSECTIONSEARCH_H

#include "Minimizer.h"

namespace AxiomLib {
namespace Optimization {

class GoldenSectionSearch : public Minimizer
{
	double phi, resphi, tau;
	
	double goldenSectionSearch(const FunctionOneVariable *function,
							   double a, double b, double fb, double c, double tau);
public:
	GoldenSectionSearch();
	
    // Инициализация параметров минимизатора из окружения
	virtual void initFromEnv(const Environment& env);
	
    // Поиск минимума заданной функции в заданных пределах
	virtual void findMinimum(const FunctionOneVariable *function, 
							 double lowerBound, double upperBound, 
							 double &bestX, double &bestF);
};

}

}

#endif // GOLDENSECTIONSEARCH_H
