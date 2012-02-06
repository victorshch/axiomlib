#include "ManagedFuzzyDataStage.h"

#include "ManagedFuzzyDataController.h"

ManagedFuzzyDataStage::ManagedFuzzyDataStage() :
    m_algorithm(&(ManagedFuzzyDataController::getInstance()->fuzzyMultiDataLearnAlgorithm)),
    m_env(&(ManagedFuzzyDataController::getInstance()->env)),
    m_calculated(true)
{ }

std::string ManagedFuzzyDataStage::getParamValue(const std::string &name) const {
	std::string result;
	if(m_myEnv.getStringParamValue(result, name)) {
		m_env->getStringParamValue(result, name);
	}
	return result;
}

void ManagedFuzzyDataStage::setParamValue(const std::string &name, const std::string &newValue) {
	if(newValue.empty()) {
		m_myEnv.undefineParam(name);
		m_env->undefineParam(name);
	} else {
		m_myEnv.setParamValue(newValue, name);
	}
}

void ManagedFuzzyDataStage::init() {
	setCalculated(true);
	
	initImpl();
}

void ManagedFuzzyDataStage::save() {
	saveImpl();
	
	*m_env += m_myEnv;
	
	m_algorithm->initFromEnv(*m_env);
}
