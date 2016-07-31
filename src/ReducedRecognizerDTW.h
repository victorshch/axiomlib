/****************************************************************************
*					ReducedRecognizerDTW
*
*	Description:	Класс для распознавания заданного образца в ряду разметки
*					с использованием алгоритма подсчета расстояния DTW.
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_ReducedRecognizerDTW_HXX
#define __AXIOMS_LIB_ReducedRecognizerDTW_HXX

#include "ReducedRecognizerDistance.h"
#include "AxiomLibException.h"
#include "Round.h"

//#include "Defines.h"
// перенесено в cxx

namespace AxiomLib {

class ReducedRecognizerDTW : public ReducedRecognizerDistance {

 private:
 
 protected:
 
	// переменная для хранения максимально допустимого расстояния между разметками при котором считать их равными
	// double precision; в родительском классе

	// переменная для хранения максимально растяжения эталонной траектории
	double stretch;
  
 public:

	// Стандартный конструктор
	ReducedRecognizerDTW (void);

	// Стандартный деструктор - удаление всех динамических объектов класса
	~ReducedRecognizerDTW (void);

	// Функция задания значений параметров класса, в том числе - классов разметки и подсчета ошибок
	int setParamsFromEnv (const Environment& env);

	// Функция запуска поиска разметки ethalon в markUp и запись результатов поиска в result
	int run (const std::vector <int> &markUp, const std::vector<int> &etalon, std::vector <double> &result);

	// Реализация функции вычисления расстояния между строками
	int countDTWDist (const std::vector <int> &markUp, const std::vector<int> &etalon, std::vector <double> &result) const;

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_ReducedRecognizerDTW_STRATEGY_HXX */
