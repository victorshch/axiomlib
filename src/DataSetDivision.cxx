#include <algorithm>

#include <boost/lexical_cast.hpp>

#include "DataSetDivision.h"

using namespace AxiomLib;

DataSetDivision::DataSetDivision(const ReferenceClassesTS &abnormalTS, const ClassTS &normalTS)
{	
	m_tsSet.resize(abnormalTS.size() + 1);
	m_tsSet[0].reserve(normalTS.size());
	for(int multiTSNo = 0; multiTSNo < normalTS.size(); multiTSNo++) {
		//m_tsSet[0][multiTSNo].reserve(normalTS[multiTSNo].size());
			m_tsSet[0].push_back(
					ClippedMultiTS(normalTS[multiTSNo],
								   IntInterval(0, normalTS[multiTSNo].length() - 1)
								   )
					);
	}
	
	for(int abType = 0; abType < abnormalTS.size(); abType++) {
		m_tsSet[abType+1].reserve(abnormalTS[abType].size());
		for(int multiTSNo = 0; multiTSNo < abnormalTS[abType].size(); multiTSNo++) {
			//m_tsSet[abType+1][multiTSNo].reserve(abnormalTS[abType][multiTSNo].size());
			m_tsSet[abType+1].push_back(
					ClippedMultiTS(abnormalTS[abType][multiTSNo],
								   IntInterval(0, abnormalTS[abType][multiTSNo].length() - 1)
					));
		}
	}
}

std::string DataSetDivision::getMultiTSName(int classNo, int indexMultiTS) const
{
	if(classNo < -1) {
		classNo = -1;
	}

	return m_tsSet[classNo + 1][indexMultiTS].get<0>().trajectoryName;
}

int DataSetDivision::getClassCount() const {
    return m_tsSet.size() - 1;
}

int DataSetDivision::getDimensionCount() const
{
    ClippedClassTSSet::const_iterator it = m_tsSet.begin();
    while (it != m_tsSet.end() && it->empty()) ++it;
    if (it == m_tsSet.end()) throw AxiomLibException("DataSetDivision::getDimensionCount(): ill-formed dataset: all divisions are empty");

    ClippedClassTS::const_iterator it2 = it->begin();
    while (it2 != it->end() && it2->get<0>().empty()) ++it2;
    if (it2 == it->end()) throw AxiomLibException("DataSetDivision::getDimensionCount(): ill-formed dataset: all classes in a division are empty");

    return (int) it2->get<0>().size();
}

int DataSetDivision::getMultiTSCount(int classNo) const {
	if(classNo < -1) {
		classNo = -1;
	}
	
	return m_tsSet[classNo + 1].size();
}

const DataSetDivision::ClippedClassTS &DataSetDivision::getClippedClassTS(int behaviourType) const {
	if(behaviourType < -1) {
		behaviourType = -1;
	}

	int behaviourIndex = behaviourType + 1;
	
	if(behaviourIndex >= m_tsSet.size()) {
		throw AxiomLibException("Invalid behaviour type index "+boost::lexical_cast<std::string>(behaviourType));
	}
	
	return m_tsSet[behaviourIndex];
}

DataSetDivision::ClippedClassTS &DataSetDivision::getClippedClassTS(int behaviourType) {
	if(behaviourType < -1) {
		behaviourType = -1;
	}

	int behaviourIndex = behaviourType + 1;
	
	if(behaviourIndex >= m_tsSet.size()) {
		throw AxiomLibException("Invalid behaviour type index "+boost::lexical_cast<std::string>(behaviourType));
	}
	
	return m_tsSet[behaviourIndex];
}


const DataSetDivision::ClippedMultiTS &DataSetDivision::getClippedMultiTS(int behaviourType, int multiTSNo) const {

	const ClippedClassTS &classTS = getClippedClassTS(behaviourType);
	
	if(multiTSNo < 0 || multiTSNo >= classTS.size()) {
		throw AxiomLibException("Invalid multi ts index "
								+boost::lexical_cast<std::string>(multiTSNo)
								+" for behaviour type "
								+boost::lexical_cast<std::string>(behaviourType));
	}
	
	return classTS[multiTSNo];
}

DataSetDivision::ClippedMultiTS &DataSetDivision::getClippedMultiTS(int behaviourType, int multiTSNo) {

	ClippedClassTS &classTS = getClippedClassTS(behaviourType);
	
	if(multiTSNo < 0 || multiTSNo >= classTS.size()) {
		throw AxiomLibException("Invalid multi ts index "
								+boost::lexical_cast<std::string>(multiTSNo)
								+" for behaviour type "
								+boost::lexical_cast<std::string>(behaviourType));
	}
	
	return classTS[multiTSNo];
}


bool DataSetDivision::getTSByIndex(std::vector<double> &v, int behaviourType, int multiTSNo, int tsNo, bool clip) const {		
	const ClippedMultiTS &cMultiTS = getClippedMultiTS(behaviourType, multiTSNo);
	
        const MultiTS &multiTS = boost::tuples::get<0>(cMultiTS);
	
	if(clip) {
                const IntInterval &clippingInterval = boost::tuples::get<1>(cMultiTS);;
		return multiTS.getTSByIndex(v, tsNo, clippingInterval.left(), clippingInterval.right());
	} else {
		return multiTS.getTSByIndex(v, tsNo);
	}
}

int DataSetDivision::getMultiTSLength(int behaviourType, int multiTSNo) const {
	const ClippedMultiTS &cMultiTS = getClippedMultiTS(behaviourType, multiTSNo);
        const MultiTS &multiTS = boost::tuples::get<0>(cMultiTS);
	return multiTS.length();
}

IntInterval DataSetDivision::getClippingInterval(int behaviourType, int multiTSNo) const {
	const ClippedMultiTS &cMultiTS = getClippedMultiTS(behaviourType, multiTSNo);
        return boost::tuples::get<1>(cMultiTS);
}

void DataSetDivision::setClippingInterval(const IntInterval &interval, int behaviourType, int multiTSNo) {
	ClippedMultiTS &cMultiTS = getClippedMultiTS(behaviourType, multiTSNo);
        boost::tuples::get<1>(cMultiTS) = interval;
}

bool DataSetDivision::getAbnormalSize(std::vector<std::vector<int> > &v) const {
	v.resize(m_tsSet.size() - 1);
	
	for(int bType = 0; bType < m_tsSet.size() - 1; bType++) {
		if(!getClassSize(bType, v[bType]))
		{
			return false;	
		}
	}
	return true;
}

bool DataSetDivision::getClassSize(int classNo, std::vector<int> &v) const {
	const ClippedClassTS &classTS = getClippedClassTS(classNo);
	
	v.clear();
	v.reserve(classTS.size());
	
		for(auto it = classTS.begin(); it!=classTS.end(); ++it)
		{
			const ClippedMultiTS& multiTS = *it;
			v.push_back(boost::tuples::get<0>(multiTS).size());
		}
	
	return true;
}

bool DataSetDivision::getNormalSize(std::vector<int> &v) const {
	return getClassSize(-1, v);
}
