#include "conditionmodelonecondition.h"

ConditionModelOneCondition::ConditionModelOneCondition(ASModelTreeItem *parent, const PCondition &condition)
	:ASModelTreeItemLeaf(parent), condition_(condition), checked_(true)
{
	
}

int ConditionModelOneCondition::columnCount() const {
	return 3;
}

QVariant ConditionModelOneCondition::data(int column) const {
	switch(column) {
	case 0: return QString::fromStdString(condition_.get()->name());
	case 1: return clusterizer_.clustersPerTraj();
	case 2: return clusterizer_.aveMaxSize();
//	case 3: return clusterizer_.goal1();
//	case 4: return clusterizer_.goal2();
	default: return QVariant();
	}
}

const Condition &ConditionModelOneCondition::condition() const {
	return *condition_;
}

Clusterizer &ConditionModelOneCondition::clusterizer() {
	return clusterizer_;
}

bool ConditionModelOneCondition::checked() const {
	return checked_;
}

void ConditionModelOneCondition::setChecked(bool value) {
	checked_ = value;
}
