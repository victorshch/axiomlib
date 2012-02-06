#include "ASModelTreeItem.h"

ASModelTreeItem::ASModelTreeItem(ASModelTreeItem *parent) :QObject(0), m_parent(parent) {}

ASModelTreeItem *ASModelTreeItem::parent() const {
	return m_parent;
}

int ASModelTreeItem::row() const {
	if(parent()) {
		return parent()->rowOfChild(this);
	} else {
		return 0;
	}
}

void ASModelTreeItem::emitItemChanged(ASModelTreeItem *item) {
	if(parent() == 0) {
		emit itemChanged(item);
	} else {
		parent()->emitItemChanged(item);
	}
}

void ASModelTreeItem::propagateChanges(ASModelTreeItem *item) {
	if(!handleChanges(item)) {
		if(parent()) {
			parent()->propagateChanges(item);
		} else {
			//throw
		}
	}
}

bool ASModelTreeItem::handleChanges(ASModelTreeItem * /*item*/) {
	return false;
}

ManagedFuzzyDataController* ASModelTreeItem::controller() const {
	if(parent() == 0) {
		return 0;
	}
	
	return parent()->controller();
}
