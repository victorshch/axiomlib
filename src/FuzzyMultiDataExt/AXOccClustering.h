/****************************************************************************
*				AXOccClustering.h
*
*	Description: Эвристика кластеризации по точкам выполнения для аксиом
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef AXOCCCLUSTERING_H
#define AXOCCCLUSTERING_H

#include "OccClustering.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class AXOccClustering : public OccClustering
{
public:
    AXOccClustering();
	
	std::string name() const;
	
	int stage() const;
	
	AXOccClustering* clone() const;
protected:
	void run(FuzzyMultiDataExtAlgorithm &algorithm, int classNo) const;
};

};

};

#endif // AXOCCCLUSTERING_H
