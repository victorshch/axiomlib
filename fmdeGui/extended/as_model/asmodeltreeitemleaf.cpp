#include "asmodeltreeitemleaf.h"

ASModelTreeItemLeaf::ASModelTreeItemLeaf(ASModelTreeItem *parent) : ASModelTreeItem(parent)
{
}

ASModelTreeItem *ASModelTreeItemLeaf::child(int /*row*/) const {
	return 0;
}

int ASModelTreeItemLeaf::childCount() const {
	return 0;
}

int ASModelTreeItemLeaf::rowOfChild(const ASModelTreeItem * /*child*/) const {
	return -1;
}
