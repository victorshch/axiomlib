#include "ReducedSatPointSet.h"

using namespace AxiomLib;
using namespace FuzzyMultiDataExt;

ReducedSatPointSet::ReducedSatPointSet(PSatPointSet normal, PSatPointSet abnormal)
	: m_normal(normal), m_abnormal(abnormal)
{ }

ReducedSatPointSet ReducedSatPointSet::deepCopy() const {
	PSatPointSet newNormal = PSatPointSet(new SatPointSet(normal()));
	PSatPointSet newAbnormal = PSatPointSet(new SatPointSet(abnormal()));
	return ReducedSatPointSet(
	            newNormal,
	            newAbnormal
	            );
}

void ReducedSatPointSet::assign(const ReducedSatPointSet &other) {
	if(this == &other) {
		return;
	}
	
	if(this->m_normal == PSatPointSet()) {
		m_normal = PSatPointSet(new SatPointSet(other.normal()));
		m_abnormal = PSatPointSet(new SatPointSet(other.abnormal()));
	} else {
		(*m_normal) = other.normal();
		(*m_abnormal) = other.abnormal();
	}
}

ReducedSatPointSet& ReducedSatPointSet::operator &=(const ReducedSatPointSet& other) {
	(*m_normal) &= other.normal();
	(*m_abnormal) &= other.abnormal();
	return *this;
}

ReducedSatPointSet& ReducedSatPointSet::operator |=(const ReducedSatPointSet& other) {
	(*m_normal) |= other.normal();
	(*m_abnormal) |= other.abnormal();
	return *this;
}

bool ReducedSatPointSet::operator ==(const ReducedSatPointSet& other) const {
	return !isNull() && !other.isNull() 
	        && normal() == other.normal() && abnormal() == other.abnormal();
}
