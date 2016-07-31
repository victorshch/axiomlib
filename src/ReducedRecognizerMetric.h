/****************************************************************************
*					ReducedRecognizerMetric
*
*	Description:	Класс для получения количества ошибок
*					распознавания для популяции систем аксиом 
*					при использовании метрики в качестве меры расстояния
*					между сравниваеми строками разметки
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_ReducedRecognizerMetric_HXX
#define __AXIOMS_LIB_ReducedRecognizerMetric_HXX

#include "ReducedRecognizerDistance.h"
#include "AxiomLibException.h"
#include "Round.h"
//#include "Defines.h"
// перенесено в cxx

namespace AxiomLib {

class ReducedRecognizerMetric : public ReducedRecognizerDistance {

 private:
	
 protected:
 
	// переменная для хранения максимально допустимого расстояния между разметками при котором считать их равными
	// double precision; в родительском классе
  
 public:

	// Стандартный конструктор
	ReducedRecognizerMetric (void);

	// Стандартный деструктор - удаление всех динамических объектов класса
	~ReducedRecognizerMetric (void);

	// Функция задания значений параметров класса, в том числе - классов разметки и подсчета ошибок
	int setParamsFromEnv (const Environment& env);

	// Функция вызывает поиска разметки ethalon в markUp и запись результатов поиска в result
	int run (const std::vector <int> &markUp, const std::vector<int> &etalon, std::vector <double> &result);

	// Функция реализует поиск разметки ethalon в markUp и запись результатов поиска в result
	int countMetricDist (const std::vector <int> &markUp, const std::vector<int> &etalon, std::vector <double> &result) const;

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_ReducedRecognizerMetric_STRATEGY_HXX */
