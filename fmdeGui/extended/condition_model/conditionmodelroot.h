#ifndef CONDITIONMODELROOT_H
#define CONDITIONMODELROOT_H

#include "../as_model/asmodeltreeitemcomposite.h"
#include "../Condition.h"

class ConditionModelRoot : public ASModelTreeItemComposite
{
public:
    ConditionModelRoot();
	
	int columnCount() const;
	QVariant data(int column) const;
	
	void addCondition(const PCondition &condition);
	void clear();
};

#endif // CONDITIONMODELROOT_H
