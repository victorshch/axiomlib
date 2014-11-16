#pragma once 

#ifndef FUZZYMULTIDATAEXT_CM_SHARKCLUSTERINGMODEL_H
#define FUZZYMULTIDATAEXT_CM_SHARKCLUSTERINGMODEL_H


#include <shark/Algorithms/Trainers/NormalizeComponentsUnitVariance.h> //normalize
#include <shark/Algorithms/KMeans.h> //k-means algorithm
#include <shark/Models/Clustering/HardClusteringModel.h>//model performing hard clustering of points
#include <shark/Models/Normalizer.h>
#include "ClusteringModel.h"
#include "../../AxiomLibException.h"
#include "boost/serialization/assume_abstract.hpp"
#include "boost/serialization/access.hpp"
#include "boost/serialization/nvp.hpp"

namespace AxiomLib {

namespace FuzzyMultiDataExt {	

class KMeansClusteringModel : public ClusteringModel {
public:	
	virtual ClusteringModel* clone(){
		auto model = new KMeansClusteringModel();
		model->setClustersCount(this->k);
		return model;
	}

	virtual void addElement(const std::vector<double>& element);
	virtual void addElements(const std::vector<std::vector<double> > & elements);
	virtual void makeClustering();
	virtual void reserve(int k);

	void setClustersCount(unsigned int k){ this->k = k; }

	virtual unsigned int getClusterNumberToElement(const std::vector<double>& element) const;
	//virtual std::vector<unsigned int> getClassNumberToElements(const std::vector<std::vector<double>>& elements);

protected:
	void setParam(std::string name, std::string value);

private:

	//объ€влени€, необходимые дл€ сериализации
	friend class boost::serialization::access;

	//функци€ сериализации - в ней должны быть указаны члены-данные,
	//характеризующие состо€ние объекта
	template<class Archive>
	void serialize(Archive & archive, const unsigned int /*version*/)
	{
		archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ClusteringModel);		
		archive & BOOST_SERIALIZATION_NVP(iters);
		archive & BOOST_SERIALIZATION_NVP(centroids);
	}


	shark::Centroids centroids;
	std::vector<shark::RealVector> data;
	std::vector<std::vector<double> > store;

	void setItersCount(unsigned int iters){ this->iters = iters; }
	int iters;

	shark::HardClusteringModel<shark::RealVector>* model;

	void clearStore();
};

};

};

#endif //FUZZYMULTIDATAEXT_CM_SHARKCLUSTERINGMODEL_H
