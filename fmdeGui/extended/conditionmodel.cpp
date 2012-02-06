#include "conditionmodel.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"
#include "QtGuiException.h"

ConditionModel::ConditionModel(ManagedFuzzyDataController *controller, QObject *parent) 
	: AbstractASModel(parent), controller(controller)
{
	this->m_rootItem = PASModelTreeItem(
			new ConditionModelRoot
			);
}

void ConditionModel::addCondition(const PCondition &condition) {
	rootItem()->addCondition(condition);
}

void ConditionModel::clusterize(int minDistance) {
	clearClusterizers();
	qDebug()<<"conditionCount() = "<<conditionCount();
	for(int i = 0; i < conditionCount(); i++) {
		const Condition &currentCond = conditionItem(i)->condition();
		Clusterizer &currentClusterizer = conditionItem(i)->clusterizer();
		
		int classNo = currentCond.abnormalType();
		
		int multiTSCount = controller->getMultiTSCount(classNo);
		qDebug()<<"multiTsCount = "<<multiTSCount;
		
		MultiVector multiTS;
		MarkupVector markup;
		
		for(int j = 0; j < multiTSCount; j++) {
			controller->getMultiTS(classNo, j, multiTS);
			
			//qDebug()<<"marking up "<<j;
			unsigned long numOccurences = currentCond.markUp(multiTS, markup);
			
			if(numOccurences > 0) {
				//qDebug()<<"clusterizing "<<j;
				currentClusterizer.clusterize(markup, minDistance);
			}
		}
	}
	reset();
}

ConditionModelRoot *ConditionModel::rootItem() const {
	ConditionModelRoot *result = dynamic_cast<ConditionModelRoot *>(m_rootItem.get());
	if(!result) {
		throw QtGuiException("Wrong condition model root item type");
	}
	return result;
}

ConditionModelOneCondition *ConditionModel::conditionItem(int index) const {
	if(index < 0 || index >= conditionCount()) {
		throw QtGuiException(QString("Wrong condition index: %1").arg(index));
	}
	
	ConditionModelOneCondition *result = dynamic_cast<ConditionModelOneCondition *>(
			m_rootItem.get()->child(index)
			);
	
	if(!result) {
		throw QtGuiException("Wrong condition model child item type");
	}
	return result;
}

int ConditionModel::conditionCount() const {
	return m_rootItem.get()->childCount();
}

const Condition &ConditionModel::condition(int row) const {
	return conditionItem(row)->condition();
}

bool ConditionModel::conditionChecked(int row) const {
	return conditionItem(row)->checked();
}

void ConditionModel::clearClusterizers() {
	for(int i = 0; i < conditionCount(); i++) {
		conditionItem(i)->clusterizer().clear();
	}
}

void ConditionModel::clear() {
	rootItem()->clear();
	reset();
}

Qt::ItemFlags ConditionModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return 0;

	if(index.column() == 0) {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;	
}

QVariant ConditionModel::data(const QModelIndex &index, int role) const {
	if(role != Qt::CheckStateRole) {
		return AbstractASModel::data(index, role);
	}
	
	if(index.column() != 0) {
		return QVariant();
	}
	
	return conditionItem(index.row())->checked() ? Qt::Checked : Qt::Unchecked;
}

bool ConditionModel::setData(const QModelIndex & index, const QVariant & value, int role) {
	if(role != Qt::CheckStateRole) {
		return AbstractASModel::setData(index, value, role);
	}
	if(!index.isValid()) {
		return false;
	}
	bool v, ok;
//	if(!(value.type() == QVariant::Bool)) {
//		return false;
//	}
	int intV;
	ok = false;
	intV = value.toInt(&ok);
	if(!ok) {
		v = value.toBool();
	} else {
		v = intV == Qt::Checked;
	}
	//v = value.toBool();
	conditionItem(index.row())->setChecked(v);
	emit dataChanged(index, index);
	return true;
}
