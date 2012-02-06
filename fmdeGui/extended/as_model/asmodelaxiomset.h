#ifndef ASMODELAXIOMSET_H
#define ASMODELAXIOMSET_H

#include "AxiomExprSetPlus.h"

#include "ASModelTreeItem.h"
#include "../algorithm_controller/forwarddecls.h"

class ASModelAxiomSet : public ASModelTreeItem
{
	Q_OBJECT
private:
	AxiomLib::AxiomExprSetPlus &as;
	PASModelTreeItem axioms;
	PASModelTreeItem markups;
	
	const ASModelTreeItem *parent;
	
	static const int m_columnCount = 1;
	
	int totalTrajNumber;
protected:
	virtual bool handleChanges(ASModelTreeItem *item);
public:
    ASModelAxiomSet(ASModelTreeItem * parent, AxiomLib::AxiomExprSetPlus &as);
	
	AxiomLib::AxiomExprSetPlus &axiomSet() { return as; }
	
	virtual ASModelTreeItem *child(int row) const;
	virtual int childCount() const;
	virtual int columnCount() const;
	virtual QVariant data(int column) const;
	virtual int rowOfChild(const ASModelTreeItem *child) const;
	
	//void refreshAxiomSet();

};

#endif // ASMODELAXIOMSET_H
