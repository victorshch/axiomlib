#ifndef CLUSTERINGMARK_H
#define CLUSTERINGMARK_H

#include "../Environment.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {	
class ClusteringMark {			
	public:
		virtual void initFromEnv(const Environment& env) = 0;
		virtual double calculate();
}
};

};