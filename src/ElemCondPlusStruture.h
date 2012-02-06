/****************************************************************************
*						ElemCondPlusStruture
*
*	Description:	Описание структуры элементарного условия
*	Author:			dk
*	History:     	-
*
****************************************************************************/

#ifndef __AXIOMS_LIB_ElemCondPlusStruture_HXX
#define __AXIOMS_LIB_ElemCondPlusStruture_HXX

#include <map>
#include <string>

namespace AxiomLib {

class ElemCondPlusStruture {

 public:

	// название элементарного условия
	std::string ecName;
	
	// Знак, с которым элементарное условие встречается в выражении, составляющем аксиому
	// Если false, то элеметарное условие идет с отрицанием
	bool sign;

	// размерность набора данных, на которой проверять данное элементарное условие
	int dimension;

	// Параметры данного элементарного условия
	std::map<std::string, std::string> ecParams;


	//   Некоторые функции класса:
	// Функция очищает содержимое полей класса
	void clear() {
		ecName.clear();
		sign = true;
		dimension = -1;
		ecParams.clear();
	}
	
}; //end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_ElemCondPlusStruture_HXX */
