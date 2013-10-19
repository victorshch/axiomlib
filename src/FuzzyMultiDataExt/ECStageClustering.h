#pragma once

#include "../Environment.h"
#include "../FuzzyDataSet.h"
#include "Common.h"
#include "ECStage.h"
#include "ForwardDeclarations.h"
#include "ClusteringFeatures/ClusteringFeature.h"
#include "ClusteringRealizations\IClusteringModel.h"
#include "ECStageClusteringElemCond.h"


#ifndef FUZZYMULTIDATAEXT_EC_CLUSTERING_H
#define FUZZYMULTIDATAEXT_EC_CLUSTERING_H

using namespace std;

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class ECStageClustering : public ECStage {
public: 
	ECStageClustering(FuzzyDataSet* fuzzyDataSet, ECTypeStage* stage0);

	virtual void setECs(const std::vector<std::vector<std::vector<std::vector<ElemCondPlusStat> > > > & value) = 0;
	
	virtual int initFromEnv(const Environment& env);
	
	virtual void run(); 

	/*
		В ф-ции run должно происходить:
		1) Взятие из траектории N участков одинаковой длины
		2) Перевод каждого взятого участка в вектор признаков (см. класс ClusteringMark)
		3) Кластеризация полученный векторов
	*/
	
	// Функции доступа к набору ЭУ
	virtual const ElemCondPlusStat & getEC(int aType, int dimension, int type, int n) const = 0;
	
	virtual bool isECSelected(int aType, int dimension, int type, int n) const = 0;
	
	virtual void setECSelected(int aType, int dimension, int type, int n, bool value) = 0;
	
	virtual void getECSize(std::vector<std::vector<std::vector<int> > > &result) const = 0;
	
	virtual int getECSize() const  = 0;
	virtual int getECSize(int aType) const  = 0;
	virtual int getECSize(int aType, int dimension) const = 0;
	virtual int getECSize(int aType, int dimension, int ecType) const = 0;
	
	virtual int getECTotalCount() const = 0;
	
	virtual void recalculateMatterECFunc(ElemCondPlusStat& ec, int abType) const = 0;


private:
	Logger* logger;

	// List of clustering features using for transform strip to vector of features
	std::list<ClusteringFeature*> clusteringFeatures;

	// List of elementary conditions
	std::list<ECStageClusteringElemCond*> elemCond;
	
	// Count of features in list "clusteringFeatures"
	int featuresCount;

	int dimensions;

	// result data after getting strips and trasforming into vector of feature 
	vector<vector<vector<double>>> resultFeatures;

	// count of strips which are randomly taken from trajectory
	int stripsCount;

	int stripLength;

	// k-means cluster's count
	int k;
	
	// Realization of clustering algorithm
	IClusteringModel** clusteringModels;

	void handleTrajectory(const std::vector<double>& trajectory, int dimension);
};

};
};
#endif // FUZZYMULTIDATAEXT_EC_CLUSTERING_H