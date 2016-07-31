/****************************************************************************
*				HeuristicsSet.h
*
*	Description: Набор эвристик алгоритма
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef HEURISTICSSET_H
#define HEURISTICSSET_H

#include <map>
#include <deque>

#include <boost/shared_ptr.hpp>

#include "../Environment.h"

#include "ForwardDeclarations.h"

#include "Heuristics.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

// Отвечает за считывание названий эвристик из конфига
// и применение их в нужные моменты в нужном порядке
class HeuristicsSet
{
public:
    HeuristicsSet();
	
	// Возвращает -1, если ни одной эвристики не задано, иначе 0
	int initFromEnv(const Environment& env);
	
	void apply(FuzzyMultiDataExtAlgorithm* algorithm, int stage) const;
private:
	typedef boost::shared_ptr<Heuristics> PHeuristics;
	typedef std::deque<PHeuristics> HeuristicsVector;
	typedef std::map<int, HeuristicsVector> HeuristicsMap;
	
	HeuristicsMap m_heuristicsMap;

	static bool compareHeuristicsOrder(PHeuristics h1, PHeuristics h2);
};

};

};

#endif // HEURISTICSSET_H
