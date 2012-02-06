#include "AxiomExprPlus.h"

#include "ASModelConjunction.h"
#include "../algorithm_controller/ManagedFuzzyDataController.h"
#include "AxiomStatistics.h"

#include "asmodeloneaxiom.h"

ASModelOneAxiom::ASModelOneAxiom(ASModelTreeItem *parent, const AxiomLib::AxiomExpr &axiomExpr, int index)
	:ASModelTreeItemComposite(parent), axiomExpr(axiomExpr), index(index)
{
	selectBestGoal();
	
	for(int i = 0; i < axiomExpr.expression.size(); i++) {
		addChild(new ASModelConjunction(this, axiomExpr.expression[i], i, this->abType));
	}
}

int ASModelOneAxiom::columnCount() const {
	return 6;
}

QVariant ASModelOneAxiom::data(int column) const {
	switch(column) {
	case 0 : return QVariant::fromValue(
			QString::number(index + 1) + " : " + QString::fromStdString(
					axiomExpr.nameOfAxiomExpr
					)
			);
	case 1 : return QVariant::fromValue(axiomExpr.goal);
	case 5: return QVariant::fromValue(axiomExpr.statOccurence);
	default: return QVariant();
	}
}

void ASModelOneAxiom::selectBestGoal() {
	int abTypeCount = controller()->fuzzyMultiDataLearnAlgorithm.getDataSet().getClassCount();
	
	controller()->fuzzyMultiDataLearnAlgorithm.recalculateMatterAxiomFunc(axiomExpr, 0);	        
	
	AxiomLib::AxiomStatistics statistics(axiomExpr);
	this->abType = 0;
	
	for(int abType = 1; abType < abTypeCount; abType++) {
		controller()->fuzzyMultiDataLearnAlgorithm.recalculateMatterAxiomFunc(axiomExpr, abType);
		if(axiomExpr.goal > statistics.goal()) {
			statistics = AxiomLib::AxiomStatistics(axiomExpr);
			this->abType = abType;
		}
	}
	
	statistics.copyTo(axiomExpr);
}
