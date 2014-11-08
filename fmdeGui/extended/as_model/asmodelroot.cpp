#include <algorithm>
#include <iterator>

#include <QtDebug>

#include "../utilities.h"

#include "asmodelroot.h"

#include "asmodelaxiomset.h"

ASModelRoot::ASModelRoot(ManagedFuzzyDataController *controller)
	: ASModelTreeItemComposite(0), m_controller(controller) {
}

void ASModelRoot::addAxiomSet(AxiomLib::AxiomExprSetPlus &as) {
	addChild(
		new ASModelAxiomSet(this, as)
				);
}

AxiomLib::AxiomExprSetPlus ASModelRoot::getAxiomSet(int row) const
{
	if(row < 0 || row >= childCount()) {
		return AxiomLib::AxiomExprSetPlus();
	}

	ASModelAxiomSet* axiomSetNode = dynamic_cast<ASModelAxiomSet*>(child(row));
	if(!axiomSetNode) {
		return AxiomLib::AxiomExprSetPlus();
	}

	return axiomSetNode->axiomSet();
}

void ASModelRoot::clear() {
	clearChildren();
}

int ASModelRoot::columnCount() const {
	return 6;
}

QVariant ASModelRoot::data(int column) const {
	switch(column) {
	case 0: return encode("Имя");
	case 1: return encode("Целев.");
	case 2: return encode("# ошибок 1-го рода");
	case 3: return encode("% ошибок 2-го рода");
	case 4: return encode("Разметка");
	case 5: return encode("Встречаемость");
	default: return QVariant();
	}	
}

bool ASModelRoot::handleChanges(ASModelTreeItem *item) {
	ASModelAxiomSet *itemAxiomSet = dynamic_cast<ASModelAxiomSet *>(item);
	if(itemAxiomSet) {
		//qDebug()<<"Entering handleChanges()";
		//qDebug()<<"Recalculating";
		m_controller->fuzzyMultiDataLearnAlgorithm.recalculateMatterAxiomSetFunc(itemAxiomSet->axiomSet());
		qDebug()<<"Exiting handleChanges()";
		return true;
	}
	return false;
}

ManagedFuzzyDataController* ASModelRoot::controller() const {
	return m_controller;
}
