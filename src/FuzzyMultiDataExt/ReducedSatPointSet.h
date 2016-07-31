/****************************************************************************
*				ReducedSatPointSet.h
*
*	Description: Набор точек выполнения для траекторий нормального поведения и траекторий одного
*				класса нештатного поведения. Используется на стадии подбора аксиом
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef REDUCEDSATPOINTSET_H
#define REDUCEDSATPOINTSET_H

#include "../SatPointSet.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class ReducedSatPointSet
{
public:
	ReducedSatPointSet() {}
    ReducedSatPointSet(PSatPointSet normal, PSatPointSet abnormal);
	
	// Конструктор копирования по умолчанию осуществляет shallow copy
	// Данный метод используется, когда нужна deep copy
	ReducedSatPointSet deepCopy() const;
	
	// Оператор присваивания по умолчанию осуществляет shallow copy
	// Данный метод используется, когда нужна deep copy
	void assign(const ReducedSatPointSet& other);
	
	ReducedSatPointSet& operator |=(const ReducedSatPointSet& other);
	ReducedSatPointSet& operator &=(const ReducedSatPointSet& other);
	
	bool operator==(const ReducedSatPointSet& other) const;
	
	const SatPointSet& normal() const { return *m_normal; }
	const SatPointSet& abnormal() const { return *m_abnormal; }
	
	bool isNull() const { return m_normal == PSatPointSet(); }
private:
	PSatPointSet m_normal;
	PSatPointSet m_abnormal;
};

}

}

#endif // REDUCEDSATPOINTSET_H
