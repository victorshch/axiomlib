/****************************************************************************
*				AXOccFilter.h
*
*	Description: Эвристика кластеризации по встречаемости для аксиом
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef AXOCCFILTER_H
#define AXOCCFILTER_H

#include "Heuristics.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class AXOccFilter : public Heuristics
{
public:
    AXOccFilter();
	
	// Имя эвристики
	virtual std::string name() const;
	
	// Стадия, на которой применяется эвристика
	virtual int stage() const;
	
	// Виртуальный конструктор копирования
	virtual Heuristics* clone() const;
	
private:
	double m_threshold;
protected:
	virtual void initFromEnvImpl(const Environment &env);
	
	// Запуск эвристики на алгоритме
	virtual void run(FuzzyMultiDataExtAlgorithm& algorithm, int aType) const;	
};

}

}

#endif // AXOCCFILTER_H
