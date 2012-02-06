#include <iterator>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include "../Logger.h"
#include "../FuzzyMultiDataExtAlgorithm.h"

#include "ECTypeSelection.h"

using namespace AxiomLib;
using namespace FuzzyMultiDataExt;

ECTypeSelection::ECTypeSelection()
{
	m_selectionPolicy = Exclusive;
}

std::string ECTypeSelection::name() const {
	return "ECTypeSelection";
}

int ECTypeSelection::stage() const {
	return 0;
}

ECTypeSelection* ECTypeSelection::clone() const {
	return new ECTypeSelection(*this);
}

//bool operator== (const std::pair<bool, std::string>& p1, const std::pair<bool, std::string>& p2) {
//	return p1.first == p2.first && p1.second == p2.second;
//}


void ECTypeSelection::run(FuzzyMultiDataExtAlgorithm &algorithm) const {
	//TODO сделать возможность указания типов ЭУ
	//отдельно для каждого класса
	Logger::getInstance()->writeComment("Starting ECTypeSelection heuristics");
	Logger::getInstance()->writeDebug("Policy: " + boost::lexical_cast<std::string>(m_selectionPolicy));
	Logger::getInstance()->writeDebug("Selected EC types count: "+boost::lexical_cast<std::string>(m_ecTypes.size()));
	for(int i = 0; i < algorithm.getECTypeSize(); i++) {
		auto pair = std::make_pair(
		            algorithm.getECType(i).sign,
		            boost::algorithm::to_lower_copy(algorithm.getECType(i).elemCondition->name())
		            );
		//todo подумать, надо ли делать эту проверку
		if(algorithm.isECTypeSelected(i)) {
			if((m_ecTypes.count(pair) != 0) == (m_selectionPolicy == Exclusive)) {
				Logger::getInstance()->writeComment("Excluding '"+algorithm.getECType(i).ecTypeName()+"'");
				algorithm.setECTypeSelected(i, false);
			} else {
				Logger::getInstance()->writeComment("Including '"+algorithm.getECType(i).ecTypeName()+"'");
			}
		}
	}
	Logger::getInstance()->writeComment("Finished ECTypeSelection heuristics");
}

inline std::string to_lower_copy(const std::string& str)
{
	return boost::algorithm::to_lower_copy(str);
}

void ECTypeSelection::initFromEnvImpl(const Environment &env) {
	Logger::getInstance()->writeDebug("Entering initFromEnvImpl for ECTypeSelection...");
	std::string policy;
	if(!env.getStringParamValue(policy, getParamName("selectionPolicy"))) {
		Logger::getInstance()->writeDebug("Policy: '"+policy+"'");
		boost::algorithm::to_lower(policy);
		if(policy == "inclusive") {
			m_selectionPolicy = Inclusive;
		} else {
			m_selectionPolicy = Exclusive;
			if(policy != "exclusive") {
				Logger::getInstance()->writeComment("Warning: invalid selection policy name, defaulting to exclusive");
			}
		}
	}
	
	std::set<std::string> ecTypeNamesInp, ecTypeNames; 
	switch(m_selectionPolicy) {
		case Exclusive: env.getStringSetParamValue(ecTypeNamesInp, getParamName("exclude")); break;
		case Inclusive: env.getStringSetParamValue(ecTypeNamesInp, getParamName("include")); break;
		default: throw AxiomLibException("Unexpected SelectionPolicy value");
	}
	
	ElemConditionsFactory factory;
	std::set<std::string> existingECNamesInp, existingECNames; 
	factory.getECNames(std::inserter(existingECNamesInp, existingECNamesInp.end()));
	
	std::transform(existingECNamesInp.begin(), existingECNamesInp.end(),
	               std::inserter(existingECNames, existingECNames.end()),
				   to_lower_copy
//	               [](const std::string& str) -> std::string
//	{ return boost::algorithm::to_lower_copy(str); }
);
	
	std::transform(ecTypeNamesInp.begin(), ecTypeNamesInp.end(),
	               std::inserter(ecTypeNames, ecTypeNames.end()),
				   to_lower_copy
//	               [](const std::string& str) -> std::string
//	{ return boost::algorithm::to_lower_copy(str); }
);
	
	for(auto it = ecTypeNames.begin(); it != ecTypeNames.end(); it++) {	
		std::string currentName = *it;
		boost::algorithm::trim(currentName);
		bool sign = true;
		Logger::getInstance()->writeDebug("Current name: '"+currentName+"'");
		if(boost::algorithm::istarts_with(currentName, "not")) {
			Logger::getInstance()->writeDebug("starts with not");
			currentName = currentName.substr(4);
			boost::algorithm::trim(currentName);
			sign = false;
			Logger::getInstance()->writeDebug("Resulting name: '"+currentName+"'");
		}
		if(existingECNames.count(currentName) > 0) {
			m_ecTypes.insert(std::make_pair(sign, currentName));
		} else {
			Logger::getInstance()->writeComment("Warinig: invalid EC type name '"+currentName+"', ignoring");
		}
	}
	Logger::getInstance()->writeDebug("Policy: " + boost::lexical_cast<std::string>(m_selectionPolicy));
	Logger::getInstance()->writeDebug("Selected EC types count: "+boost::lexical_cast<std::string>(m_ecTypes.size()));	
	Logger::getInstance()->writeDebug("Leaving initFromEnvImpl for ECTypeSelection...");
}

bool ECTypeSelection::compare::operator ()(const std::pair<bool, std::string>& p1, const std::pair<bool, std::string>& p2) const {
	if(p1.first == p2.first) {
		return p1.second < p2.second;
	} else {
		return p2.first;
	}
}
