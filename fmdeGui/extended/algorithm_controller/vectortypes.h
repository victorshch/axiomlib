#ifndef VECTORTYPES_H
#define VECTORTYPES_H

#include <vector>
#include <string>

#include "forwarddecls.h"

typedef std::vector<std::vector<std::vector<std::vector<
		AxiomLib::ElemCondPlusStat
		> > > > ECMultiVector;

typedef std::vector<std::vector<std::vector<std::vector<
		std::string
		> > > > ECNamesMultiVector;

typedef std::vector<std::vector<std::vector<std::vector<
		bool
		> > > > ECSelectedMultiVector;

typedef std::vector<std::vector<AxiomLib::AxiomExprPlus> > AxiomMultiVector;
typedef std::vector<std::vector<std::string> > AxiomNamesMultiVector;
typedef std::vector<std::vector<bool> > AxiomSelectedMultiVector;

typedef std::vector<AxiomLib::AxiomExprSetPlus> AxiomSetVector;

#endif // VECTORTYPES_H
