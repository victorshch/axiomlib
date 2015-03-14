#pragma once 

#ifndef FUZZYMULTIDATAEXT_CM_SHARKCLUSTERINGMODEL_H
#define FUZZYMULTIDATAEXT_CM_SHARKCLUSTERINGMODEL_H

#include <shark/Algorithms/KMeans.h> //k-means algorithm
#include <shark/Models/Clustering/HardClusteringModel.h>//model performing hard clustering of points
#include <shark/LinAlg/Base.h>
#include "../../AxiomLibException.h"
#include "ClusteringModel.h"
#include <boost/serialization/nvp.hpp>

namespace AxiomLib {

namespace FuzzyMultiDataExt {	

class KMeansClusteringModel : public ClusteringModel {
public:	
	virtual ClusteringModel* clone(){
        KMeansClusteringModel* model = new KMeansClusteringModel();
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
    friend class boost::serialization::access;

    template<class Archive>
    void save(Archive & archive, const unsigned int version) const
    {
        boost::serialization::base_object<ClusteringModel>(*this);
        archive & BOOST_SERIALIZATION_NVP(iters);
        archive & BOOST_SERIALIZATION_NVP(dimension);

        // ???????? ????????? ? std::vector<std::vector<double> >
        shark::Data<shark::RealVector> centroidsData = centroids.centroids();

        std::vector<std::vector<double> > serializable;

        for(shark::Data<shark::RealVector>::const_element_range::iterator it = centroidsData.elements().begin();
            it != centroidsData.elements().end(); ++it) {
            shark::RealVector currentCentroid = *it;

            serializable.push_back(std::vector<double>());
            for(unsigned i = 0; i < currentCentroid.size(); ++i) {
                serializable.back().push_back(currentCentroid[i]);
            }
        }

        archive & BOOST_SERIALIZATION_NVP(serializable);
    }

    template<class Archive>
    void load(Archive & archive, const unsigned int version)
    {
        boost::serialization::base_object<ClusteringModel>(*this);

        archive & BOOST_SERIALIZATION_NVP(iters);
        archive & BOOST_SERIALIZATION_NVP(dimension);

        std::vector<std::vector<double> > serializable;

        archive & BOOST_SERIALIZATION_NVP(serializable);

        std::vector<size_t> batchSizes = shark::detail::optimalBatchSizes(serializable.size(), shark::Data<shark::RealVector>::DefaultBatchSize);
        shark::Data<shark::RealVector> restoredCentroidsData = shark::Data<shark::RealVector>(batchSizes.size());

        int currentRow = 0;
        for(size_t b = 0; b != batchSizes.size(); ++b) {
           shark::RealMatrix& batch = restoredCentroidsData.batch(b);
           batch.resize(batchSizes[b], dimension);
           // copy the rows into the batch
           for(size_t i = 0; i != batchSizes[b]; ++i,++currentRow){
               // TODO check row size

               for(size_t j = 0; j != dimension; ++j){
                   batch(i,j) = serializable[currentRow][j];
               }
           }
        }

        centroids.setCentroids(restoredCentroidsData);
        model = new shark::HardClusteringModel<shark::RealVector>(&centroids);
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

	shark::Centroids centroids;
	std::vector<shark::RealVector> data;
	std::vector<std::vector<double> > store;

	void setItersCount(unsigned int iters){ this->iters = iters; }
	int iters;
    int dimension;

	shark::HardClusteringModel<shark::RealVector>* model;

	void clearStore();
};

};

};

#endif //FUZZYMULTIDATAEXT_CM_SHARKCLUSTERINGMODEL_H
