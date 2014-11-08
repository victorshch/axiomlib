#ifndef ASMODELROOT_H
#define ASMODELROOT_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "asmodeltreeitemcomposite.h"
#include "AxiomExprSetPlus.h"
#include "../algorithm_controller/ManagedFuzzyDataController.h"

class ASModelRoot : public ASModelTreeItemComposite
{
	Q_OBJECT
private:
	ManagedFuzzyDataController* m_controller;
protected:
	bool handleChanges(ASModelTreeItem *item);
	
	virtual ManagedFuzzyDataController* controller() const;
public:
    ASModelRoot(ManagedFuzzyDataController *m_controller);
	
	void addAxiomSet(AxiomLib::AxiomExprSetPlus& as);

	AxiomLib::AxiomExprSetPlus getAxiomSet(int row) const;
	
	void clear();
	
	virtual int columnCount() const;
	virtual QVariant data(int column) const;
};

typedef boost::shared_ptr<ASModelRoot> PASModelRoot;

#endif // ASMODELROOT_H
