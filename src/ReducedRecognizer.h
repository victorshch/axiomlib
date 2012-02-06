/****************************************************************************
*					ReducedRecognizer
*
*	Description:	Абстрактный класс для получения количества ошибок
*					распознавания для заданных рядов разметки
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_ReducedRecognizer_HXX
#define __AXIOMS_LIB_ReducedRecognizer_HXX

#include "Environment.h"
#include <vector>

namespace AxiomLib {

class ReducedRecognizer {

 private:
 
 protected:
  
 public:
 
	// Стандартный конструктор - пустой
	// ReducedRecognizer (void) {}
	
	// Стандартный деструктор - пустой. Необходим, чтобы вызывались деструкторы классов, реализующих данный интерфейс
	virtual ~ReducedRecognizer (void) {}
	
	// Функция задания значений параметров класса
	virtual int setParamsFromEnv (const Environment& env) = 0;
	
	// Функция поиска разметки ethalon в markUp и запись результатов поиска в result
	virtual int run (const std::vector <int> &markUp, const std::vector<int> &etalon, std::vector <int> &result) = 0;
	
	// Функция поиска разметки ethalon в markUp -- в result пишутся получающиеся расстояния
	virtual int run(const std::vector <int> &markUp, const std::vector<int> &etalon, std::vector <double> &result) = 0;
	
  
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_ReducedRecognizer_STRATEGY_HXX */
