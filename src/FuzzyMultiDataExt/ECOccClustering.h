/****************************************************************************
*				ECOccClustering.h
*
*	Description: Эвристика кластеризации по точкам выполнения для ЭУ
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef ECOCCCLUSTERING_H
#define ECOCCCLUSTERING_H

#include "OccClustering.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class ECOccClustering : public OccClustering
{
public:
    ECOccClustering();
	
	std::string name() const;
	
	int stage() const;
	
	ECOccClustering* clone() const;
	
protected:
	void run(FuzzyMultiDataExtAlgorithm &algorithm, int classNo) const;
};

};

};

#endif // ECOCCCLUSTERING_H
