#include "abstractasmodel.h"

AbstractASModel::AbstractASModel(QObject *parent) : QAbstractItemModel(parent)
{
}

ASModelTreeItem *AbstractASModel::getItemByIndex(const QModelIndex &index) const {
	if(index.isValid()) {
		return static_cast<ASModelTreeItem *>(index.internalPointer());
	} else {
		return m_rootItem.get();
	}
}

QModelIndex AbstractASModel::getIndexByItem(ASModelTreeItem *item, int column) const {
	return createIndex(item->row(), column , item);
}

QVariant AbstractASModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();
	
	ASModelTreeItem *item = getItemByIndex(index);
	
	return item->data(index.column());
}

Qt::ItemFlags AbstractASModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;	
}

QVariant AbstractASModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return m_rootItem->data(section);

	return QVariant();
}

QModelIndex AbstractASModel::index(int row, int column, const QModelIndex &parent) const {

	 if (!hasIndex(row, column, parent))
		 return QModelIndex();

	 ASModelTreeItem *parentItem;

	 parentItem = getItemByIndex(parent);

	 ASModelTreeItem *childItem = parentItem->child(row);
	 if (childItem)
		 return createIndex(row, column, childItem);
	 else
		 return QModelIndex();

}

QModelIndex AbstractASModel::parent(const QModelIndex &index) const {
	if (!index.isValid()) {
		return QModelIndex();
	}

	ASModelTreeItem *childItem = getItemByIndex(index);
	
	if(childItem == m_rootItem.get()) {
		return QModelIndex();
	}
	
	ASModelTreeItem *parentItem = childItem->parent();

	if (parentItem == m_rootItem.get()) {
		return QModelIndex();
	}

	return createIndex(parentItem->row(), 0, parentItem);
}

int AbstractASModel::rowCount(const QModelIndex &parent) const {
    ASModelTreeItem *parentItem;
     if (parent.column() > 0)
         return 0;

     parentItem = getItemByIndex(parent);

     return parentItem->childCount();
	
}

int AbstractASModel::columnCount(const QModelIndex &parent) const {
	return getItemByIndex(parent)->columnCount();
}
