#include "as_model/asmodelroot.h"
#include "as_model/asmodelonemarkup.h"
#include "markupeditor.h"
#include "asmodel.h"

ASModel::ASModel(ManagedFuzzyDataController *controller, QObject *parent) :
    AbstractASModel(parent)
{
	ASModelRoot *rootItemTyped = new ASModelRoot(controller);
	m_rootItem = PASModelTreeItem(
				rootItemTyped
			);
	
	connect(rootItemTyped, SIGNAL(itemChanged(ASModelTreeItem*)), 
			this, SLOT(itemChanged(ASModelTreeItem*)));
}

void ASModel::itemClicked(const QModelIndex &index) {
	ASModelTreeItem *item = getItemByIndex(index);
	ASModelOneMarkup *itemMarkup = dynamic_cast<ASModelOneMarkup *>(item);
	if(itemMarkup) {
		MarkupEditor *markupEditor = new MarkupEditor(itemMarkup->markup(), itemMarkup->maxAxiomNumber(), 0);
		connect(markupEditor, SIGNAL(markupChanged(std::vector<int>)), 
				itemMarkup, 
				SLOT(changeMarkup(std::vector<int>)));
		
		markupEditor->show();
	}
}

void ASModel::itemChanged(ASModelTreeItem *item) {
	emit dataChanged(
			getIndexByItem(item, 0),
			getIndexByItem(item, item->columnCount() - 1)
			);
}

ASModelRoot* ASModel::root() const {
	return dynamic_cast<ASModelRoot*>(m_rootItem.get());
}

void ASModel::addAxiomSet(AxiomLib::AxiomExprSetPlus &as) {
	beginResetModel();
	
	root()->addAxiomSet(as);
	
	endResetModel();
}

void ASModel::clear() {
	beginResetModel();
	root()->clear();
	endResetModel();
}

AxiomLib::AxiomExprSetPlus ASModel::getAxiomSet(int row)
{
	return root()->getAxiomSet(row);
}
