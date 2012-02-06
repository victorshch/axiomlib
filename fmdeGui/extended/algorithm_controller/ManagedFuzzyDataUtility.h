#ifndef MANAGEDFUZZYDATAUTILITY_H
#define MANAGEDFUZZYDATAUTILITY_H

#include "FuzzyMultiDataExtAlgorithm.h"

// TODO implement functions

template<class Function>
void forEachECType(AxiomLib::FuzzyMultiDataExtAlgorithm* algorithm, Function function) {
	for(int i = 0; i < algorithm->getECTypeSize(); i++) {
		function(i, algorithm->getECType(i), algorithm->isECTypeSelected(i));
	}
}

template<class Function>
void forEachEC(AxiomLib::FuzzyMultiDataExtAlgorithm* algorithm, Function function) {
	std::vector<std::vector<std::vector<int> > > sizeVector;
	algorithm->getECSize(sizeVector);
	
	for(int i = 0; i < sizeVector.size(); i++) {
		for(int j = 0; j < sizeVector[i].size(); j++) {
			for(int k = 0; k < sizeVector[i][j].size(); k++) {
				for(int l = 0; l < sizeVector[i][j][k]; l++ ) {
					function(i, j, k, l, algorithm->getEC(i, j, k , l), algorithm->isECSelected(i, j, k, l));
				}
			}
		}
	}
}

template<class Function>
void forEachAxiom(AxiomLib::FuzzyMultiDataExtAlgorithm* algorithm, Function function) {
	for(int i = 0; i < algorithm->getAXSize(); i++) {
		for(int j = 0; j < algorithm->getAXSize(i); j++) {
			function(i, j, algorithm->getAX(i, j), algorithm->isAXSelected(i, j));
		}
	}
}

template<class Function>
void forEachAxiomSet(AxiomLib::FuzzyMultiDataExtAlgorithm* algorithm, Function function) {
	for(int i = 0; i < algorithm->getASSize(); i++) {
		function(i, algorithm->getAS(i));
	}
}

#endif // MANAGEDFUZZYDATAUTILITY_H
