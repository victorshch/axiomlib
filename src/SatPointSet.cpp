#include <algorithm>

#include "SatPointSet.h"

using namespace AxiomLib;

SatPointSet::SatPointSet(const ElemCondPlus &ecPlus, const ClippedFullFuzzyDataSet &dataSet, ClippedFullFuzzyDataSet::DataSetDivisionType division, int classNo) {
	std::vector<int> sizeVector;
	dataSet.getSizeForClass(division, classNo, sizeVector);
	
	int multiTSCount = sizeVector.size();
	
	m_satPoints.resize(multiTSCount);

	std::vector<double> ts;
	
	for(int multiTSNo = 0; multiTSNo < multiTSCount; multiTSNo++) {
		dataSet.getTSByIndex(division, ts, classNo, multiTSNo, ecPlus.dimension);
		
		int multiTSSize = ts.size();
		
		m_satPoints[multiTSNo].resize(multiTSSize);
		
		for(int i = 0; i < multiTSSize; i++) {
			m_satPoints[multiTSNo][i] = (SatValue) (ecPlus.check(i, ts) == 1);
		}
	}		
}

SatPointSet::SatPointSet(const AxiomExpr &axiom, const ClippedFullFuzzyDataSet &dataSet, ClippedFullFuzzyDataSet::DataSetDivisionType division, int classNo) {
	std::vector<int> sizeVector;
	dataSet.getSizeForClass(division, classNo, sizeVector);
	
	int multiTSCount = sizeVector.size();
	
	m_satPoints.resize(multiTSCount);

	std::vector<std::vector<double> > multiTS;

	int dimensionCount = dataSet.getDimensionCount();
	
	std::vector<bool> dimensions(dimensionCount);
	
	axiom.getDimensions(dimensions);
	
	for(int multiTSNo = 0; multiTSNo < multiTSCount; multiTSNo++) {
		multiTS.resize(dimensionCount);
		for(int tsNo = 0; tsNo < dimensionCount; tsNo++) {
			if(dimensions[tsNo]) {
				dataSet.getTSByIndex(division, multiTS[tsNo], classNo, multiTSNo, tsNo);
			}
		}
		
		int multiTSLen = (int) dataSet.getMultiTSLength(division, classNo, multiTSNo);
		
		m_satPoints[multiTSNo].resize(multiTSLen);
		
		for(int i = 0; i < multiTSLen; i++) {
			m_satPoints[multiTSNo][i] = (SatValue) (axiom.check(i, multiTS) == 1);
		}
	}	
}

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
