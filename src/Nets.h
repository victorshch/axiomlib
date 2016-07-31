/****************************************************************************
*			Nets
*
*	Description:	Класс содержащий массив нейросетей. 
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Nets_HXX
#define __AXIOMS_LIB_Nets_HXX

#include "Environment.h"
#include "AxiomSetPop.h"
//#include "AxiomSetPopExt.h"
#include "RefMarkupSetSimple.h"
#include "RefMarkupSimple.h"
#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

class Nets {

 private:

	 // Переменная хранит статус класса - если сети обучались, то стоит true, иначе false.
	 bool status; 

 protected:
 
 public:
 
	// Стандартный конструктор - задание значений по умолчанию
	Nets (void);

	// Стандартный деструктор - пустой
	~Nets ();

	// Функция обучения массива сетей по заданным разметкам для каждого класса аномального поведения
	int prepare (std::vector <RefMarkupSetSimple> &refMarkupSetSimple);

	// Функция проверки - есть ли в ряду markUp разметка из запомненных ранее, причем которая заканчивается в ряду на индексе i
	double check (int &resNum, std::vector <int> &row, long int &i) const;
  
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_Nets_STRATEGY_HXX */
