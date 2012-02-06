#ifndef CONDITIONMODEL_H
#define CONDITIONMODEL_H

#include <deque>
#include <utility>

#include "abstractasmodel.h"
#include "Condition.h"
#include "condition_model/conditionmodelroot.h"
#include "condition_model/conditionmodelonecondition.h"
#include "algorithm_controller/forwarddecls.h"

//typedef std::deque<std::pair<PCondition, Clusterizer> > ConditionList;

class ConditionModel : public AbstractASModel
{
Q_OBJECT
public:
    ConditionModel(ManagedFuzzyDataController *controller, QObject *parent = 0);
	
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
	
	void addCondition(const PCondition &condition);
	int conditionCount() const;
	const Condition &condition(int row) const;
	bool conditionChecked(int row) const;
	void clear();
	
	void clusterize(int minDistance);
private:
	ManagedFuzzyDataController *controller;
	
	ConditionModelRoot *rootItem() const;
	ConditionModelOneCondition *conditionItem(int index) const;

	void clearClusterizers();	
};

#endif // CONDITIONMODEL_H
