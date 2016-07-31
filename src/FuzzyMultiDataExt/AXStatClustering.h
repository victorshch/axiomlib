/****************************************************************************
*				AXStatClustering.h
*
*	Description: Эвристика кластеризации по статистическим характеристикам для аксиом
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef AXSTATCLUSTERING_H
#define AXSTATCLUSTERING_H

#include "StatClustering.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class AXStatClustering : public StatClustering
{
public:
    AXStatClustering();
	
	// Имя эвристики
	virtual std::string name() const;
	
	// Стадия, на которой применяется эвристика
	virtual int stage() const;
	
	// Виртуальный конструктор копирования
	virtual AXStatClustering* clone() const;
	
protected:
	// Запуск эвристики на алгоритме
	virtual void run(FuzzyMultiDataExtAlgorithm& algorithm, int classNo) const;
	
};

}

}

#endif // AXSTATCLUSTERING_H
