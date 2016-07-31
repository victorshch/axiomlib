/****************************************************************************
*				NeuralCoin.h
*
*	Description:	Файл, содержащий определение функции coin() и класса Coiner
*	Author:		wictor
*	History:	
*
****************************************************************************/

#ifndef COIN_H_
#define COIN_H_

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>

namespace AxiomLib {


//Функция возвращает true с вероятностью p, иначе false. Желательно перед ее использованием сделать srand(time(0))
inline bool coin(double p=0.5);

/****************************************************************************
*				Coiner
*
*	Description:	Класс - обертка для vector<bool>, ведущая себя как урна 
*					с белыми (true) и черными (false) шарами
*	Author:		wictor
*	History:	
*
****************************************************************************/
class Coiner
{
	// Вектор
	std::vector<bool> v;
	//size_t curr;
public:
	// Конструктор
	Coiner(size_t num, double p);
	// Функция возвращает i-й элемент булевского вектора, причем доля 
	// элементов true в нем задана изначально, но их места случайны
	bool coin(int i) const;
};

};

#endif /* COIN_H_ */
