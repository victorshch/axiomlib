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

	virtual void setECs(const std::vector<std::vector<std::vector<std::vector<ElemCondPlusStat> > > > & value) = 0;
	
	virtual int initFromEnv(const Environment& env);
	
	virtual void run(); 

	/*
		ч Ж-ГЙЙ run ДПМЦОП РТПЙУИПДЙФШ:
		1) чЪСФЙЕ ЙЪ ФТБЕЛФПТЙЙ N ХЮБУФЛПЧ ПДЙОБЛПЧПК ДМЙОЩ
		2) рЕТЕЧПД ЛБЦДПЗП ЧЪСФПЗП ХЮБУФЛБ Ч ЧЕЛФПТ РТЙЪОБЛПЧ (УН. ЛМБУУ ClusteringMark)
		3) лМБУФЕТЙЪБГЙС РПМХЮЕООЩК ЧЕЛФПТПЧ
	*/
	
	// жХОЛГЙЙ ДПУФХРБ Л ОБВПТХ ьх
	// aType - тип нештатного поведения
	// dimension - размерность
	// type - тип элементарного условия (алгоритм кластеризации + набор фич + длина и колво участков)
	// n - Номер Э условия
	virtual const ElemCondPlusStat & getEC(int aType, int dimension, int type, int n) const = 0;
	
	virtual bool isECSelected(int aType, int dimension, int type, int n) {
		return true;
	}
	
	virtual void setECSelected(int aType, int dimension, int type, int n, bool value) {

	}
	
	virtual void getECSize(std::vector<std::vector<std::vector<int> > > &result) {
		// TODO: 
	}
	
	virtual int getECSize() const  = 0; // Количество типов нештатного поведения
	virtual int getECSize(int aType) const  = 0; 
	virtual int getECSize(int aType, int dimension) const = 0;
	virtual int getECSize(int aType, int dimension, int ecType) const = 0;
	
	// TODO: определить в родителе
	virtual int getECTotalCount() const = 0;
	
	virtual void recalculateMatterECFunc(ElemCondPlusStat& ec, int abType) {
	}
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