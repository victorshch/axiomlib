#include <iterator>
#include <algorithm>

#include "../utilities.h"

#include "asmodelaxioms.h"
#include "asmodeloneaxiom.h"

ASModelAxioms::ASModelAxioms(ASModelTreeItem *parent, const std::vector<AxiomLib::AxiomExpr *> &axioms)
	: ASModelTreeItemComposite(parent) {
	for(unsigned int i = 0; i < axioms.size(); i++) {
		addChild(new ASModelOneAxiom(this, *axioms[i], (int)i));
	}
}

int ASModelAxioms::columnCount() const {
	return 6;
}

QVariant ASModelAxioms::data(int column) const {
	if(column == 0) {
		return QVariant::fromValue(encode("Аксиомы"));
	} else {
		return QVariant();
	}
}
