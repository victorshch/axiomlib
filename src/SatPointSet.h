/****************************************************************************
*			SatPointSet.h
*
*	Description: Описание класса, предназначенного для хранения набора точек 
*				выполнения ЭУ или аксиомы
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef SATPOINTSET_H
#define SATPOINTSET_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "ClippedFullFuzzyDataSet.h"
#include "AxiomExpr.h"
#include "ElemCondPlus.h"

namespace AxiomLib {

//Набор точек выполнимости для некоторого класса
class SatPointSet
{
public:
	// Создание набора точек выполнимости по ЭУ и датасету
    SatPointSet(const ElemCondPlus &ecPlus, const ClippedFullFuzzyDataSet &dataSet, ClippedFullFuzzyDataSet::DataSetDivisionType division, 
				int classNo);
	
	// Создание набора точек выполнимости по аксиоме и датасету
	SatPointSet(const AxiomExpr &axiom, const ClippedFullFuzzyDataSet &dataSet, ClippedFullFuzzyDataSet::DataSetDivisionType division, int classNo);
		
	// Число траекторий
	int size() const { return m_satPoints.size(); }
	
	// Число точек (длина) траектории
	int size(int multiTSNo) const { return m_satPoints[multiTSNo].size(); }

	// Выполнена ли аксиома в точке траектории
	bool isSat(int multiTSNo, int i) const { return (bool)m_satPoints[multiTSNo][i]; }
	
	// Оператор слияния по &
	SatPointSet &operator&=(const SatPointSet& other);
	
	// Оператор слияния по |
	SatPointSet &operator|=(const SatPointSet& other);
	
	bool operator == (const SatPointSet& other) const;
	
	// Число точек, на которых аксиома выполняется
	int numSat(int multiTSNo) const;
	
private:
	typedef char SatValue;
	typedef std::vector<SatValue> SatVector;
	typedef std::vector<SatVector> ClassSatVector;
	
	ClassSatVector m_satPoints;
};

typedef boost::shared_ptr<SatPointSet> PSatPointSet;

};

#endif // SATPOINTSET_H
