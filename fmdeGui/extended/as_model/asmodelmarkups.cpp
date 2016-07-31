#include "../utilities.h"

#include "asmodelmarkups.h"
#include "asmodelonemarkup.h"

ASModelMarkUps::ASModelMarkUps(ASModelTreeItem *parent, const AxiomLib::AxiomExprSetPlus& as)
	: ASModelTreeItemComposite(parent) {
	
	for(unsigned int i = 0; i < as.markUps.size(); i++) {
		addChild(
				new ASModelOneMarkup(this, as, (int)i)
				);
	}
}

int ASModelMarkUps::columnCount() const {
	return 5;
}

QVariant ASModelMarkUps::data(int column) const {
	if(column == 0) {
		return QVariant::fromValue(encode("Разметки"));
	} else {
		return QVariant();
	}
}
