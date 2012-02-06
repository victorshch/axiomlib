#include "asmodelaxiomset.h"
#include "asmodelaxioms.h"
#include "asmodelmarkups.h"
#include "asmodeloneaxiom.h"
#include "asmodelonemarkup.h"
#include "../algorithm_controller/ManagedFuzzyDataController.h"

ASModelAxiomSet::ASModelAxiomSet(ASModelTreeItem *parent, AxiomLib::AxiomExprSetPlus &as) 
	: as(as), ASModelTreeItem(parent) {
	axioms = PASModelTreeItem(
			new ASModelAxioms(this, as.axioms)
			);
	markups = PASModelTreeItem(
			new ASModelMarkUps(this, as)
			);
	
	totalTrajNumber = controller()->getTotalAbnormalTrajNumber();
}

ASModelTreeItem *ASModelAxiomSet::child(int row) const {
	switch(row) {
		case 0: return axioms.get();
		case 1: return markups.get();
		default: return 0;
	}
}

int ASModelAxiomSet::childCount() const {
	return 2;
}

int ASModelAxiomSet::rowOfChild(const ASModelTreeItem *child) const {
	if(child == axioms.get()) {
		return 0;
	} else if(child == markups.get()) {
		return 1;
	} else {
		return -1;
	}
}

int ASModelAxiomSet::columnCount() const {
	return m_columnCount;
}

QVariant ASModelAxiomSet::data(int column) const {
	switch(column) {
	case 0: return QVariant::fromValue(QString::fromStdString(
				as.name()
				));
	case 1: return QVariant::fromValue(as.goal);
	case 2: return QVariant::fromValue(as.errFirst);
	//case 3: return QVariant::fromValue((double)(as.errSecond * 100) / (double)totalTrajNumber);
	case 3: return QString("%1 (%2)").arg(QString::number((double)(as.errSecond * 100) / (double)totalTrajNumber), QString::number(as.errSecond));
	default: return QVariant();
	}
}

bool ASModelAxiomSet::handleChanges(ASModelTreeItem *item) {
	ASModelOneMarkup *itemMarkup = dynamic_cast<ASModelOneMarkup *>(item);
	if(itemMarkup) {
		int index = itemMarkup->index();
		axiomSet().markUps[index] = itemMarkup->markup();
		propagateChanges(this);
		return true;
	}
	return false;
}
