#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>

#include "ModelNode.h"
#include "Schema.h"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
	// Takes ownership of root
    TreeModel(ModelNode* root, const Schema* schema, QObject *parent = 0);
	~TreeModel();

	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	
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
	
private:
	ModelNode* m_root;
	const Schema* m_schema;
	
	ModelNode* indexToNode(const QModelIndex& index) const;
	QModelIndex nodeToIndex(ModelNode* node, int column = 0) const;
	
	void establishConnections(ModelNode* node);
	
private slots:
	void nodeModified(ModelNode* node);
	void nodeChildrenModified(ModelNode* node);
	
	void nodeChildrenAdding(ModelNode* node, int row, int count);
	void nodeChildrenAdded();
	
	void nodeChildrenRemoving(ModelNode* node, int row, int count);
	void nodeChildrenRemoved();
};

#endif // TREEMODEL_H
