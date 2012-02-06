#ifndef CONDITIONMODELONECONDITION_H
#define CONDITIONMODELONECONDITION_H

#include "../as_model/asmodeltreeitemleaf.h"
#include "../Condition.h"
#include "../clusterizer.h"

class ConditionModelOneCondition : public ASModelTreeItemLeaf
{
	PCondition condition_;
	Clusterizer clusterizer_;
	
	bool checked_;
public:
    ConditionModelOneCondition(ASModelTreeItem *parent, const PCondition &condition);
	
	virtual int columnCount() const;
	virtual QVariant data(int column) const;	
	
	const Condition &condition() const;
	Clusterizer &clusterizer();
	bool checked() const;
	void setChecked(bool value);
};

#endif // CONDITIONMODELONECONDITION_H
