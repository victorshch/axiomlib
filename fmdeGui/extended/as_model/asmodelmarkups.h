#ifndef ASMODELMARKUPS_H
#define ASMODELMARKUPS_H

#include <vector>

#include "AxiomExprSetPlus.h"

#include "asmodeltreeitemcomposite.h"

class ASModelMarkUps : public ASModelTreeItemComposite
{
	Q_OBJECT
private:
	int maxAxiomNo;
public:
    ASModelMarkUps(ASModelTreeItem *parent, const AxiomLib::AxiomExprSetPlus& as);
	
	virtual int columnCount() const;
	virtual QVariant data(int column) const;
	
};

#endif // ASMODELMARKUPS_H
