#pragma once 

#include <shark/Algorithms/Trainers/NormalizeComponentsUnitVariance.h> //normalize
#include <shark/Algorithms/KMeans.h> //k-means algorithm
#include <shark/Models/Clustering/HardClusteringModel.h>//model performing hard clustering of points
#include <shark/Models/Normalizer.h>
#include "IClusteringModel.h"

#ifndef FUZZYMULTIDATAEXT_CM_SHARKCLUSTERINGMODEL_H
#define FUZZYMULTIDATAEXT_CM_SHARKCLUSTERINGMODEL_H


namespace AxiomLib {

namespace FuzzyMultiDataExt {	

class SharkClusteringModel : public IClusteringModel {

public:	
	virtual void addElement(const std::vector<double>& element);
	virtual void addElements(const std::vector<std::vector<double>> & elements);
	virtual void makeClustering();
	virtual void reserve(int k);

	virtual unsigned int getClusterNumberToElement(const std::vector<double>& element) const;
	//virtual std::vector<unsigned int> getClassNumberToElements(const std::vector<std::vector<double>>& elements);
private:
	shark::Centroids centroids;
	std::vector<shark::RealVector> data;
	std::vector<std::vector<double>> store;

	shark::HardClusteringModel<shark::RealVector>* model;

	void clearStore();
};

};

};

#endif //FUZZYMULTIDATAEXT_CM_SHARKCLUSTERINGMODEL_H
