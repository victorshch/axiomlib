/****************************************************************************
*					MinimizerFactory
*
*	Description:	Фабрика минимизаторов
*	Author:			wictor
*	History:
*
****************************************************************************/
#ifndef MINIMIZERFACTORY_H
#define MINIMIZERFACTORY_H

#include <string>

#include "Minimizer.h"

namespace AxiomLib {
namespace Optimization {

class MinimizerFactory
{
public:
	MinimizerFactory();
	
	Minimizer* create(const std::string& name);
};

}

}

#endif // MINIMIZERFACTORY_H
