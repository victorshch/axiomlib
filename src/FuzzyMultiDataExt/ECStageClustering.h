#pragma once

#ifndef FUZZYMULTIDATAEXT_EC_CLUSTERING_H
#define FUZZYMULTIDATAEXT_EC_CLUSTERING_H

#include "../Environment.h"
#include "../FuzzyDataSet.h"
#include "Common.h"
#include "ECStage.h"
#include "ForwardDeclarations.h"
#include "ClusteringFeatures/ClusteringFeature.h"
#include "ClusteringRealizations\ClusteringModel.h"
#include "ElemCondClustering.h"

using namespace std;

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class ECStageClustering : public ECStage {
public: 
	ECStageClustering(FuzzyDataSet* fuzzyDataSet, ECTypeStage* stage0);
	
	virtual int initFromEnv(const Environment& env);
	
	virtual void run(); 

	/*
		В ф-ции run должно происходить:
		1) Взятие из траектории N участков одинаковой длины
		2) Перевод каждого взятого участка в вектор признаков (см. класс ClusteringMark)
		3) Кластеризация полученный векторов
	*/
	
	// Функции доступа к набору ЭУ
	// aType - РХО МЕЬРЮРМНЦН ОНБЕДЕМХЪ
	// dimension - ПЮГЛЕПМНЯРЭ
	// type - РХО ЩКЕЛЕМРЮПМНЦН СЯКНБХЪ (ЮКЦНПХРЛ ЙКЮЯРЕПХГЮЖХХ + МЮАНП ТХВ + ДКХМЮ Х ЙНКБН СВЮЯРЙНБ)
	// n - мНЛЕП щ СЯКНБХЪ
private:
	Logger* logger;

	// List of clustering features using for transform strip to vector of features
	std::list<ClusteringFeature*> clusteringFeatures;

	// List of elementary conditions
	std::list<ElemCondClustering*> elemCond;
	
	// Count of features in list "clusteringFeatures"
	int featuresCount;

	int dimensions;

	// result data after getting strips and trasforming into vector of feature 
	vector<vector<vector<double> > > resultFeatures;

	// count of strips which are randomly taken from trajectory
	int stripsCount;

	int stripLength;

	// k-means cluster's count
	int k;
	
	// Realization of clustering algorithm
	std::vector<ClusteringModel*> clusteringModels;

	void handleTrajectory(const std::vector<double>& trajectory, int dimension);
};

};
};
#endif // FUZZYMULTIDATAEXT_EC_CLUSTERING_H