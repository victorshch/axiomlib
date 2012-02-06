/****************************************************************************
*				HeuristicsFactory.h
*
*	Description: Фабрика эвристик
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef HEURISTICSFACTORY_H
#define HEURISTICSFACTORY_H

#include <string>
#include <map>
#include <vector>

#include "ForwardDeclarations.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class HeuristicsFactory
{
public:
	HeuristicsFactory();
	~HeuristicsFactory();
	
    Heuristics* createHeuristics(const std::string& name) const;
	//std::vector<std::string> getHeuristicsNames() const;
	
private:
	
	typedef std::map<std::string, Heuristics*> HeuristicsMap;
	
	// Прототипы эвристик
	HeuristicsMap m_prototypes;
	
	void addPrototype(Heuristics* heuristics);

	static void cleanUpMapEntry(const HeuristicsMap::value_type& v);
};

}

}

#endif // HEURISTICSFACTORY_H
