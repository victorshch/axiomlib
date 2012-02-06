#include <boost/algorithm/string.hpp>
#include <algorithm>

#include "../AxiomLibException.h"
#include "Heuristics.h"

#include "ECStatClustering.h"
#include "AXStatClustering.h"
#include "ECOccFilter.h"
#include "AXOccFilter.h"
#include "ECTypeSelection.h"
#include "ECOccClustering.h"
#include "AXOccClustering.h"

#include "HeuristicsFactory.h"

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

HeuristicsFactory::HeuristicsFactory() {
	//TODO вызовы addPrototype для каждого типа эвристики
	addPrototype(new ECStatClustering());
	addPrototype(new AXStatClustering());
	addPrototype(new ECOccFilter());
	addPrototype(new AXOccFilter());
	addPrototype(new ECTypeSelection());
	addPrototype(new ECOccClustering());
	addPrototype(new AXOccClustering());
}


void HeuristicsFactory::cleanUpMapEntry(const HeuristicsMap::value_type &v){
	delete v.second;
}

HeuristicsFactory::~HeuristicsFactory() {
	std::for_each(m_prototypes.begin(), m_prototypes.end(), 
				  cleanUpMapEntry
	              );
}

Heuristics* HeuristicsFactory::createHeuristics(const std::string &name) const {
	std::string lowercaseName = boost::algorithm::to_lower_copy(name);
	HeuristicsMap::const_iterator it = m_prototypes.find(lowercaseName);
	
	if(it == m_prototypes.end()) {
		return 0;
	}
	
	return it->second->clone();
}

void HeuristicsFactory::addPrototype(Heuristics *heuristics) {
	if(heuristics == 0) {
		throw AxiomLibException("HeuristicsFactory::addPrototype() : heuristics cannot be null");
	}
	
	std::string lowercaseName = boost::algorithm::to_lower_copy(
	            heuristics->name()
	                );
	HeuristicsMap::const_iterator it = m_prototypes.find(
	            lowercaseName
	            );
	if(it != m_prototypes.end()) {
		throw AxiomLibException("Heuristics name conflict: " + heuristics->name());
	}
	
	m_prototypes.insert(std::make_pair(lowercaseName, heuristics));
}
