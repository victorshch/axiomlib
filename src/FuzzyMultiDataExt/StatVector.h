/****************************************************************************
*				StatVector.h
*
*	Description: Набор статистических характеристик ЭУ или аксиомы
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef STATVECTOR_H
#define STATVECTOR_H

namespace AxiomLib {

namespace FuzzyMultiDataExt {

const int statVectorDim = 5;

class StatVector
{
public:
    StatVector(double d1, double d2, double d3, double d4, double d5 = 0.0);
	
	static double distance(const StatVector& v1, const StatVector& v2);
	
private:
	
	double v[statVectorDim];
};

};

};

#endif // STATVECTOR_H
