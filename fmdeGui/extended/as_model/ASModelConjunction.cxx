#include "ASModelEC.h"

#include "ASModelConjunction.h"

ASModelConjunction::ASModelConjunction(ASModelTreeItem *parent, const std::vector<AxiomLib::ElemCondPlus> &elemConds, int index, int abType)
	:ASModelTreeItemComposite(parent), m_index(index)
{
	for(int i = 0; i < elemConds.size(); i++) {
		addChild(
		            new ASModelEC(this, elemConds[i], abType)
		            );
	}
}

int ASModelConjunction::columnCount() const {
	return 6;
}

QVariant ASModelConjunction::data(int column) const {
	switch(column) {
	case 0: return QVariant(QString("Conjunction %1").arg(m_index));
	default: return QVariant();
	}
}
