#ifndef LABELINGSTRATEGYFACTORY_H
#define LABELINGSTRATEGYFACTORY_H

#include <string>

#include "LabelingStrategy.h"

namespace AxiomLib {

class LabelingStrategyFactory
{
public:
	LabelingStrategyFactory();
	
	LabelingStrategy* create(const std::string& name) const;
};

}

#endif // LABELINGSTRATEGYFACTORY_H
