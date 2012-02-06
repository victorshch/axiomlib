#ifndef ABSTRACTASMODEL_H
#define ABSTRACTASMODEL_H

#include <QAbstractItemModel>
#include "as_model/asmodelroot.h"

class AbstractASModel : public QAbstractItemModel
{
Q_OBJECT

protected:
	PASModelTreeItem m_rootItem;
	ASModelTreeItem *getItemByIndex(const QModelIndex &index) const;
	QModelIndex getIndexByItem(ASModelTreeItem *item, int column = 0) const;	
public:
    AbstractASModel(QObject *parent = 0);

	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
signals:

public slots:

};
#endif // ABSTRACTASMODEL_H
