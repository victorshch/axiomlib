#ifndef ASMODELCONJUNCTION_H
#define ASMODELCONJUNCTION_H

#include <vector>
#include "asmodeltreeitemcomposite.h"

class ASModelConjunction : public ASModelTreeItemComposite
{
public:
    ASModelConjunction(ASModelTreeItem *parent, const std::vector<AxiomLib::ElemCondPlus>& elemConds, int index, int abType);
	
	int columnCount() const;
	QVariant data(int column) const;
	
private:
	int m_index;
};

#endif // ASMODELCONJUNCTION_H
