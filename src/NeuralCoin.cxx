#include "NeuralCoin.h"

namespace AxiomLib {

/****************************************************************************
*					coin
*
*	Description: Функция возвращает true с вероятностью p, иначе false. Желательно перед ее использованием сделать srand(time(0))
*	Parameters:	p - вероятность возвратить true, по умолчанию 0.5
*	Returns:	true с вероятностью p, иначе false
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
inline bool coin(double p) {
	return p>=1? true : (p<=0 ? false :((long double)(std::rand()%RAND_MAX)/(long double)(RAND_MAX)) < p);
}


/****************************************************************************
*					Coiner::Coiner
*
*	Description: Конструктор
*	Parameters:	num - общее число элементов (шаров)
*				p - доля true (белых шаров)
*	Returns:	-
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
Coiner::Coiner(size_t num, double p):v(num, false)
{
	std::srand((unsigned) std::time(0));
	
	// clip the probability to [0, 1]
	if(p>1) p=1;
	if(p<0) p=0;

	
	double p1 = num*p;

	size_t n = static_cast<size_t>(std::floor(p1));

	// дробная часть произведения доли белых шаров на их общее число
	double p2 = p1 - n;

	for(size_t i=0; i<n; i++)
	{
		v[i]=true;
	}

	if(n<num)
	{
		// с этой вероятностью добавляем еще один белый шар
		if(AxiomLib::coin(p2)) v[n] = true;
	}
	
	// Перемешиваем шары случайным образом
	std::random_shuffle(v.begin(), v.end());
}

/****************************************************************************
*					Coiner::coin
*
*	Description: Функция возвращает i-й элемент булевского вектора, причем доля 
*				элементов true в нем задана изначально, но их места случайны
*	Parameters:	i - позиция в векторе. Должна быть меньше num, заданного в конструкторе
*	Returns:	true либо false
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
bool Coiner::coin(int i) const
{
	if(i >= (int) v.size() || i < 0) return false;
	return v[i];
}

};