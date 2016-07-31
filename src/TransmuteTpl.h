/****************************************************************************
*					TransmuteTpl
*
*	Description:	Класс мутации для алгоритма обчения шаблонов
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Transmute_Tpl_HXX
#define __AXIOMS_LIB_Transmute_Tpl_HXX

#include "Environment.h"
#include "Axiom.h"
#include "AxiomSet.h"
#include "AxiomSetPlus.h"
#include "AxiomLibException.h"
#include "Round.h"
#include <string>
#include <vector>

namespace AxiomLib {

class TransmuteTpl {

 private:

	/*std::vector < std::vector <Axiom *>* > axioms;

	std::vector < std::vector <double> > axiomsProfit;

	std::vector < std::vector <unsigned short> > axiomsMask;

	std::vector < std::vector <double> > axiomsMaskProb;
	*/
	
	// Если true, то изменение порядка аксиом является возможной мутацией. 
	// Иначе изменение порядка исключено из возможных мутаций
	bool canMutateOrder;

 protected:
  
 public:

	// Стандартный конструктор - пустой
	TransmuteTpl ();

	// Стандартный деструктор - пустой
	~TransmuteTpl ();

	// Возвращает имя данного класса мутации
	std::string name (void) const;

	// Функция задания значений параметров класса из окружения
	int setParamsFromEnv (const Environment& env);

	// Функция сортировки вектора vec по невозрастанию с заносом результатов в виде индексов в вектор mask
	inline void sort (std::vector <double> &vec, std::vector <unsigned short> &mask) const;

	// Функция мутации отдельной системы аксиом
	int transmute (AxiomSetPlus &axiomSetPlus) const;

	// Провести операцию мутации для элементов популяции
	int transmute (std::vector <AxiomSetPlus> &population) const;
  
}; // end of class

}; // end of namespace

#endif /* __AXIOMS_LIB_Transmute_Tpl_HXX */
