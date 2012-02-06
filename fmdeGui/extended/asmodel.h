#ifndef ASMODEL_H
#define ASMODEL_H

#include <vector>

#include "abstractasmodel.h"
#include "AxiomExprSetPlus.h"
#include "algorithm_controller/forwarddecls.h"

class ASModel : public AbstractASModel
{
Q_OBJECT
private:
	ASModelRoot* root() const;
private slots:
	void itemClicked(const QModelIndex &modelIndex);
	void itemChanged(ASModelTreeItem *item);
public:
    ASModel(ManagedFuzzyDataController *controller,
					 QObject *parent = 0);

	void addAxiomSet(AxiomLib::AxiomExprSetPlus& as);
	void clear();
signals:

public slots:

};

#endif // ASMODEL_H
