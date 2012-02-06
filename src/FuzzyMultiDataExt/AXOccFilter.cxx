#include "../FuzzyMultiDataExtAlgorithm.h"

#include "AXOccFilter.h"

using namespace AxiomLib;
using namespace FuzzyMultiDataExt;

const double epsilon = 1e-6;

AXOccFilter::AXOccFilter()
{
	setOrder(2);
	m_threshold = 1.0;
}

std::string AxiomLib::FuzzyMultiDataExt::AXOccFilter::name() const {
	return "AXOccFilter";
}

int AxiomLib::FuzzyMultiDataExt::AXOccFilter::stage() const {
	return 2;
}

AxiomLib::FuzzyMultiDataExt::Heuristics * AxiomLib::FuzzyMultiDataExt::AXOccFilter::clone() const {
	return new AXOccFilter(*this);
}

void AxiomLib::FuzzyMultiDataExt::AXOccFilter::run(AxiomLib::FuzzyMultiDataExtAlgorithm &algorithm, int aType) const {
	for(int n = 0; n  < algorithm.getAXSize(aType); n++) {
		if(algorithm.isAXSelected(aType, n)) {
			algorithm.setAXSelected(aType, n, 
									(algorithm.getAX(aType, n).statOccurence > m_threshold - epsilon)
									);
		}
	}
}

void AxiomLib::FuzzyMultiDataExt::AXOccFilter::initFromEnvImpl(const AxiomLib::Environment &env) {
	env.getParamValue(m_threshold, getParamName("threshold"));
}

