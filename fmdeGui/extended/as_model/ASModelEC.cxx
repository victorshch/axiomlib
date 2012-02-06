#include "ElemCondPlusStat.h"
#include "../algorithm_controller/ManagedFuzzyDataController.h"

#include "ASModelEC.h"

ASModelEC::ASModelEC(ASModelTreeItem *parent, const AxiomLib::ElemCondPlus &ec, int abType)
	: ASModelTreeItemLeaf(parent)
{
	AxiomLib::ElemCondPlusStat ecStat(ec);
	controller()->fuzzyMultiDataLearnAlgorithm.recalculateMatterECFunc(ecStat, abType);
	m_stat = AxiomLib::ECStatistics(ecStat);
	m_name = ec.ecTypeName();
}

int ASModelEC::columnCount() const {
	return 6;
}

QVariant ASModelEC::data(int column) const {
	switch(column) {
	case 0: return QVariant(QString::fromStdString(m_name));
	case 1: return QVariant::fromValue(m_stat.goal());
	case 5: return QVariant::fromValue(m_stat.statOccurrence());
	default: return QVariant();
	}
}
