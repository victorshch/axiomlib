#ifndef ASMODELEC_H
#define ASMODELEC_H

#include <string>

#include "ECStatistics.h"

#include "asmodeltreeitemleaf.h"

class ASModelEC : public ASModelTreeItemLeaf
{
public:
    ASModelEC(ASModelTreeItem *parent, const AxiomLib::ElemCondPlus& ec, int abType);
	
	int columnCount() const;
	QVariant data(int column) const;
private:
	AxiomLib::ECStatistics m_stat;
	std::string m_name;
};

#endif // ASMODELEC_H
