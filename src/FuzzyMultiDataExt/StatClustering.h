/****************************************************************************
*				StatClustering.h
*
*	Description: Базовый класс для эвристик кластеризации по статистическим 
*				характеристикам
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef STATCLUSTERING_H
#define STATCLUSTERING_H

#include <boost/tuple/tuple.hpp>

#include "Heuristics.h"
#include "StatVector.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class StatClustering : public Heuristics
{
	double m_radius;
	
	// Сколько оставлять с одинаковой статистикой
	int m_retain;
	
protected:
	virtual void initFromEnvImpl(const Environment &env);
	
	void clusterize(const std::vector<StatVector>& v, std::vector<bool>& result) const;
	
	int radius() const;
	int retain() const;
public:
    StatClustering();
	
};

};

};

#endif // STATCLUSTERING_H
