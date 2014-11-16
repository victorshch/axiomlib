#pragma once

#ifndef FUZZYMULTIDATAEXT_CM_CLUSTERINGMODEL_H
#define FUZZYMULTIDATAEXT_CM_CLUSTERINGMODEL_H

#include <vector>
#include <string>
#include "boost/serialization/assume_abstract.hpp"
#include "boost/serialization/access.hpp"
#include "boost/serialization/nvp.hpp"


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

private:
	//объ€влени€, необходимые дл€ сериализации
	friend class boost::serialization::access;

	//функци€ сериализации - в ней должны быть указаны члены-данные,
	//характеризующие состо€ние объекта
	template<class Archive>
	void serialize(Archive & archive, const unsigned int /*version*/)
	{
		archive & BOOST_SERIALIZATION_NVP(k);
	}
};

};

};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(AxiomLib::ClusteringModel);

#endif //FUZZYMULTIDATAEXT_CM_CLUSTERINGMODEL_Hb
