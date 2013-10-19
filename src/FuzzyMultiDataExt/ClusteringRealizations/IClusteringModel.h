#pragma once

#include <vector>

#ifndef FUZZYMULTIDATAEXT_CM_ICLUSTERINGMODEL_H
#define FUZZYMULTIDATAEXT_CM_ICLUSTERINGMODEL_H

namespace AxiomLib {

namespace FuzzyMultiDataExt {	

class IClusteringModel {
public:
	void setClustersCount(unsigned int k){ this->k = k; }
	unsigned int getClustersCount() { return this->k; }

	virtual void reserve(int k) = 0;
	virtual void addElement(const std::vector<double>& element) = 0;
	virtual void addElements(const std::vector<std::vector<double>>& elements) = 0;
	virtual void makeClustering() = 0;

	virtual unsigned int getClusterNumberToElement(const std::vector<double>& element) const = 0;
	//virtual std::vector<unsigned int> getClassNumberToElements(const std::vector<std::vector<double>>& elements) = 0;
protected: 
	bool wasTrained;

	// clusters count
	unsigned int k;
};

};

};

#endif //FUZZYMULTIDATAEXT_CM_ICLUSTERINGMODEL_H