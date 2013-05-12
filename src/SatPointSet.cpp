#include <algorithm>

#include "SatPointSet.h"

using namespace AxiomLib;

SatPointSet& SatPointSet::operator &=(const SatPointSet& other) {
	// TODO: вставить проверку размеров при дебаге
	
	for(int i = 0; i < m_satPoints.size(); i++) {
		SatVector &current = m_satPoints[i];
		const SatVector &otherCurrent = other.m_satPoints[i];
		for(int j = 0; j < current.size(); j++) {
			current[j] &= otherCurrent[j];
		}
	}
	
	return *this;
}

SatPointSet& SatPointSet::operator |=(const SatPointSet& other) {
	// TODO: вставить проверку размеров при дебаге
	
	for(int i = 0; i < m_satPoints.size(); i++) {
		SatVector &current = m_satPoints[i];
		const SatVector &otherCurrent = other.m_satPoints[i];
		for(int j = 0; j < current.size(); j++) {
			current[j] |= otherCurrent[j];
		}
	}
	
	return *this;
}

bool SatPointSet::operator ==(const SatPointSet& other) const {
	if(m_satPoints.size() != other.m_satPoints.size()) {
		return false;
	}
	for(int i = m_satPoints.size(); i < other.m_satPoints.size(); ++i) {
		const SatVector &current = m_satPoints[i];
		const SatVector &otherCurrent = other.m_satPoints[i];		
		if(current.size() != otherCurrent.size()) {
			return false;
		}
		for(int j = 0; j < current.size(); j++) {
			if(current[j] != otherCurrent[j]) {
				return false;
			}
		}
	}
	return true;
}

int SatPointSet::numSat(int multiTSNo) const {
	const SatVector& current = m_satPoints[multiTSNo];
	
	return std::count(current.begin(), current.end(),
						 (SatVector::value_type) true
	);
}
