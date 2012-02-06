#include <set>
#include <string>
#include <utility>
#include <algorithm>

#include <boost/foreach.hpp>

#include "../AxiomLibException.h"
#include "../Logger.h"
#include "HeuristicsFactory.h"

#include "HeuristicsSet.h"

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

HeuristicsSet::HeuristicsSet()
{
}

bool HeuristicsSet::compareHeuristicsOrder(PHeuristics h1, PHeuristics h2) {
	return h1->order() < h2->order();
}

int HeuristicsSet::initFromEnv(const Environment &env) {
	m_heuristicsMap.clear();
	
	std::set<std::string> heuristicsNameSet;
	
	if(env.getStringSetParamValue(heuristicsNameSet, "Heuristics") == -1) {
		return -1;
	}
	
	HeuristicsFactory heuristicsFactory;
	
	BOOST_FOREACH(const std::string& str, heuristicsNameSet) {
		Heuristics* current = heuristicsFactory.createHeuristics(str);
		if(current == 0) {
			throw AxiomLibException("Incorrect heuristics name: "+str);
		}
		Logger::getInstance()->writeDebug("Initializing heuristics "+str);
		current->initFromEnv(env);
		m_heuristicsMap[current->stage()].push_back(PHeuristics(current));
	}
	
	BOOST_FOREACH(HeuristicsMap::value_type& pair, m_heuristicsMap) {
		std::sort(pair.second.begin(), pair.second.end(), 
				  compareHeuristicsOrder
		);
	}

	return 0;
}

void HeuristicsSet::apply(FuzzyMultiDataExtAlgorithm *algorithm, int stage) const {
	HeuristicsMap::const_iterator it = m_heuristicsMap.find(stage);
	if(it == m_heuristicsMap.end()) {
		return;
	}
	
	BOOST_FOREACH(PHeuristics h, it->second) {
		h->run(*algorithm);
	}
}
