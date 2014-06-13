#pragma once

#ifndef FUZZYMULTIDATAEXT_CM_CLUSTERINGMODEL_H
#define FUZZYMULTIDATAEXT_CM_CLUSTERINGMODEL_H

#include <vector>
#include <string>

namespace AxiomLib {

namespace FuzzyMultiDataExt {	

class ClusteringModel {
public:	
	static ClusteringModel* create(const std::string& s);
	 
	unsigned int getClustersCount() { return this->k; }

	virtual ClusteringModel* clone() = 0;
	virtual void reserve(int k) = 0;
	virtual void addElement(const std::vector<double>& element) = 0;
	virtual void addElements(const std::vector<std::vector<double> >& elements) = 0;
	virtual void makeClustering() = 0;

	virtual unsigned int getClusterNumberToElement(const std::vector<double>& element) const = 0;
	//virtual std::vector<unsigned int> getClassNumberToElements(const std::vector<std::vector<double>>& elements) = 0;
protected: 
	virtual void setParam(std::string name, std::string value) = 0;

	bool wasTrained;

	// clusters count
	unsigned int k;
};

};

};

#endif //FUZZYMULTIDATAEXT_CM_CLUSTERINGMODEL_H
