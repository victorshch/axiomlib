#ifndef ASMODELONEAXIOM_H
#define ASMODELONEAXIOM_H

#include "AxiomExprPlus.h"

#include "asmodeltreeitemcomposite.h"

class ASModelOneAxiom : public ASModelTreeItemComposite
{
	Q_OBJECT
private:
	AxiomLib::AxiomExprPlus axiomExpr;
	int index;
	int abType;
	
	// Находит номер класса нештатного поведения, для которого получается наибольшая
	// целевая функция
	void selectBestGoal();
public:
    ASModelOneAxiom(ASModelTreeItem *parent, const AxiomLib::AxiomExpr & axiomExpr, int index);
	
	AxiomLib::AxiomExpr &axiom() { return axiomExpr; }
	
	virtual int columnCount() const;
	virtual QVariant data(int column) const;
};

#endif // ASMODELONEAXIOM_H
