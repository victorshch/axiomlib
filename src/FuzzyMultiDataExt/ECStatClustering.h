/****************************************************************************
*				ECStatClustering.h
*
*	Description: Эвристика кластеризации по статистическим характеристикам для ЭУ
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef ECSTATCLUSTERING_H
#define ECSTATCLUSTERING_H

#include "StatClustering.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class ECStatClustering : public StatClustering
{
public:
    ECStatClustering();
	
	// Имя эвристики
	virtual std::string name() const;
	
	// Стадия, на которой применяется эвристика
	virtual int stage() const;
	
	// Виртуальный конструктор копирования
	virtual ECStatClustering* clone() const;
	
protected:	
	// Запуск эвристики на алгоритме
	virtual void run(FuzzyMultiDataExtAlgorithm& algorithm, int abType) const;
};

}

}

#endif // ECSTATCLUSTERING_H
