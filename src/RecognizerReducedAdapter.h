/****************************************************************************
*					RecognizerReducedAdapter
*
*	Description:	Класс-адаптер, позволяющий применять ReducedRecognizer
*					там, где нужен Recognizer
*	Author:			wictor
*	History:	
*
****************************************************************************/
#ifndef RECOGNIZERREDUCEDADAPTER_H
#define RECOGNIZERREDUCEDADAPTER_H

#include <boost/shared_ptr.hpp>

#include "RecognizerImplBase.h"
#include "ReducedRecognizer.h"
#include "TrajectorySampleDistance.h"

namespace AxiomLib {

class RecognizerReducedAdapter : public RecognizerImplBase
{
private:
	boost::shared_ptr<ReducedRecognizer> reducedRecognizer;
	
public:
	RecognizerReducedAdapter();
	
	// Задание параметров класса-потомка
	virtual int initFromEnv (const Environment& env);
	
	// Функция строит маркировки тестовых траекторий датасета, т. е. указывает для каждой из них участки нештатного поведения и их типы
	virtual void computeDistances(AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, 
								 std::vector<TrajectorySampleDistance >& result, 
								 // какие аксиомы срабатывали при разметке эталонных траекторий
								 std::vector<bool> &refAxiomUsage, 
								 // какие аксиомы срабатывали при разметке тестовых траекторий
								 std::vector<bool> &testAxiomUsage);
	
	// Функция возвращает копию распознавателя
    virtual Recognizer* copy ();
	
};

}

#endif // RECOGNIZERREDUCEDADAPTER_H
