#ifndef ASMODELAXIOMS_H
#define ASMODELAXIOMS_H

#include "asmodeltreeitemcomposite.h"
#include <vector>
#include "AxiomExpr.h"

typedef boost::shared_ptr<AxiomLib::AxiomExpr> PAxiomExpr;

class ASModelAxioms : public ASModelTreeItemComposite
{
	Q_OBJECT
private:
public:
    ASModelAxioms(ASModelTreeItem *parent, const std::vector < AxiomLib::AxiomExpr* >& axioms);
	
	virtual int columnCount() const;
	virtual QVariant data(int column) const;
};

#endif // ASMODELAXIOMS_H
